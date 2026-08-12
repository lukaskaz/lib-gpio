#include "gpio/interfaces/rpi/native/switch.hpp"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>

#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <unordered_map>

namespace gpio::rpi::native::switches
{

using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace gpio::helpers;

struct Switch::Handler
{
  public:
    explicit Handler(const config_t& config) :
        logif{std::get<2>(config)}, mode{std::get<0>(config)}
    {
        const auto& pins{std::get<1>(config)};
        switch (mode)
        {
            case modetype::input:
                std::ranges::for_each(pins, [this](auto pin) {
                    switches.try_emplace(pin, this, pin);
                });

                useasync([this, running = running.get_token()]() {
                    try
                    {
                        log(logs::level::info, "Switches monitoring started");
                        while (!running.stop_requested())
                        {
                            std::ranges::for_each(switches, [this](auto& pin) {
                                pin.second.monitor();
                            });
                            usleep((uint32_t)monitorinterval.count());
                        }
                    }
                    catch (const std::exception& ex)
                    {
                        log(logs::level::error, ex.what());
                        throw;
                    }
                });
                break;
            case modetype::tristate:
                throw std::runtime_error(
                    "Mode for given switches not supported" +
                    std::to_string((int32_t)mode));
        }
    }

    ~Handler()
    {
        running.request_stop();
        switch (mode)
        {
            case modetype::input:
                log(logs::level::info, "Switches operation ended");
                break;
            case modetype::tristate:
                // yet not supported
                break;
        }
    }

    bool observe(int32_t switchpin, std::shared_ptr<Observer<GpioData>> obs)
    {
        if (switches.contains(switchpin))
        {
            std::ostringstream oss;
            oss << std::hex << obs.get();
            log(logs::level::debug, "Adding observer "s + oss.str() +
                                        " for switch pin " +
                                        std::to_string(switchpin));
            switches.at(switchpin).subscribe(obs);
            return true;
        }
        return false;
    }

    bool unobserve(int32_t switchpin, std::shared_ptr<Observer<GpioData>> obs)
    {
        if (switches.contains(switchpin))
        {
            std::ostringstream oss;
            oss << std::hex << obs.get();
            log(logs::level::debug, "Removing observer "s + oss.str() +
                                        " for switch pin " +
                                        std::to_string(switchpin));
            switches.at(switchpin).unsubscribe(obs);
            return true;
        }
        return false;
    }

    bool read(int32_t pin, uint8_t& val)
    {
        if (switches.contains(pin))
        {
            auto& in{switches.at(pin)};
            val = in.read();
            return true;
        }
        return false;
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;
    // Chip 0 on older Pi models, chip 4 on Pi 5
    const std::filesystem::path gpiochippath{"/dev/gpiochip4"};
    const modetype mode;
    class SwitchInput : public Observable<GpioData>
    {
      public:
        SwitchInput(Switch::Handler* handler, int32_t switchpin) :
            handler{handler}, switchpin{switchpin}
        {
            initialize();
            handler->log(logs::level::info,
                         "Created switch input: " + std::to_string(switchpin));
        }
        ~SwitchInput()
        {
            deinitialize();
            handler->log(logs::level::info,
                         "Removed switch input: " + std::to_string(switchpin));
        }

        bool monitor()
        {
            bool ret{};
            switch (getpendingevent())
            {
                case Event::none:
                    if (isnotifyneeded() &&
                        getdelay(switchedlast) >= notifydelay)
                        ret = notifyclients();
                    break;
                case Event::pressed:
                    switchedlast = getcurrent();
                    break;
                case Event::released:
                    ret = handleevent(getdelay(switchedlast) >= longpressdelay);
                    switchedlast = getcurrent();
                    break;
            }
            return ret;
        }

        uint8_t read() const
        {
            struct gpiohandle_data data;
            if (ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) != 0)
                throw std::runtime_error(
                    "Cannot get switch pin " + std::to_string(switchpin) +
                    " due to ioctl error: " + strerror(errno));
            return data.values[0];
        }

      private:
        enum class Event
        {
            none = 0,
            pressed = GPIOEVENT_EVENT_RISING_EDGE,
            released = GPIOEVENT_REQUEST_FALLING_EDGE
        };
        const Switch::Handler* handler;
        const int32_t switchpin;
        const std::chrono::milliseconds notifydelay{500ms};
        const std::chrono::milliseconds longpressdelay{1s};
        // const std::chrono::milliseconds bouncedelay{200ms};
        // const std::chrono::microseconds monitorinterval{10ms};
        uint32_t longpressnum{};
        uint32_t switchednum{};
        std::chrono::steady_clock::time_point switchedlast{};
        int32_t fd;

        bool initialize()
        {
            struct gpioevent_request req;
            req.lineoffset = switchpin;
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;
            req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            // req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
            // req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            // req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            req.handleflags =
                GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
            strcpy(req.consumer_label, "rpi_input");

            auto ifs = fopen(handler->gpiochippath.c_str(), "r");
            if (!ifs)
                throw std::runtime_error(
                    "Cannot open gpio chip file for switch pin: " +
                    handler->gpiochippath.native());

            if (ioctl(fileno(ifs), GPIO_GET_LINEEVENT_IOCTL, &req) != 0)
                throw std::runtime_error(
                    "Cannot initialize switch pin " +
                    std::to_string(switchpin) +
                    " due to ioctl error: " + strerror(errno));

            fclose(ifs);
            fd = req.fd;
            handler->log(logs::level::debug,
                         "Obtained switch pin[" + std::to_string(switchpin) +
                             "] file descriptor " + std::to_string(fd));

            auto flags = fcntl(fd, F_GETFL);
            flags |= O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
                throw std::runtime_error(
                    "Cannot initialize switch pin " +
                    std::to_string(switchpin) +
                    " due to fcntl error: " + strerror(errno));

            return true;
        }

        bool deinitialize()
        {
            close(fd);
            return true;
        }

        Event getpendingevent() const
        {
            auto event{Event::none}, validevent{Event::none};
            while ((event = waitforevent(0)) != Event::none)
                validevent = event;
            return validevent;
        }

        Event waitforevent(int32_t delayms) const
        {
            struct pollfd polls;
            struct gpioevent_data evdata;

            polls.fd = fd;
            polls.events = POLLIN | POLLERR;
            polls.revents = 0;

            auto pollret = poll(&polls, 1, delayms);
            if (pollret < 0)
            {
                throw std::runtime_error(
                    "Cannot monitor pin " + std::to_string(switchpin) +
                    " due to poll error: " + strerror(errno));
            }
            else if (pollret > 0)
            {
                handler->log(logs::level::debug, "Got interrupt from switch[" +
                                                     std::to_string(switchpin) +
                                                     "] file descriptor " +
                                                     std::to_string(fd));

                auto readsize = ::read(fd, &evdata, sizeof(evdata));
                if (readsize < 0)
                {
                    int32_t readretry{10};
                    while (--readretry &&
                           (readsize = ::read(fd, &evdata, sizeof(evdata))) < 0)
                        ;
                    if (!readretry)
                        throw std::runtime_error(
                            "Cannot monitor switch pin " +
                            std::to_string(switchpin) + " due to read error (" +
                            std::to_string(readsize) + "): " + strerror(errno));
                }
                handler->log(
                    logs::level::debug,
                    "Read for switch pin[" + std::to_string(switchpin) +
                        "] data size: " + std::to_string(readsize) +
                        " event size: " + std::to_string(sizeof(evdata)));
                if (readsize == sizeof(evdata))
                {
                    handler->log(logs::level::debug,
                                 "Read for switch pin[" +
                                     std::to_string(switchpin) +
                                     "] id: " + std::to_string(evdata.id));
                    return (Event)evdata.id;
                }
            }
            else
            {
                // pollret == 0, no event detected
            }
            return Event::none;
        }

        bool handleevent(bool longpress)
        {
            longpress ? longpressnum++ : switchednum++;
            return true;
        }

        bool resetevent()
        {
            switchednum = 0;
            longpressnum = 0;
            return true;
        }

        bool isnotifyneeded() const
        {
            return switchednum || longpressnum;
        }

        bool notifyclients()
        {
            bool ret{};
            if ((ret = notify({switchpin, switchednum, longpressnum})))
                handler->log(logs::level::debug,
                             "Pin[" + std::to_string(switchpin) +
                                 "] clients notified, events num: " +
                                 std::to_string(switchednum) + "/" +
                                 std::to_string(longpressnum));
            else
                handler->log(logs::level::warning,
                             "Pin[" + std::to_string(switchpin) +
                                 "] cannot notify clients, events num: " +
                                 std::to_string(switchednum) + "/" +
                                 std::to_string(longpressnum));
            return ret & resetevent();
        }

        std::chrono::milliseconds
            getdelay(std::chrono::steady_clock::time_point prev) const
        {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                getcurrent() - prev);
        }

        std::chrono::steady_clock::time_point getcurrent() const
        {
            return std::chrono::steady_clock::now();
            return std::chrono::steady_clock::now();
        }
    };

    std::unordered_map<int32_t, SwitchInput> switches;
    std::future<void> async;
    std::stop_source running;
    const std::chrono::microseconds monitorinterval{100ms};

    void log(
        logs::level level, const std::string& msg,
        const std::source_location loc = std::source_location::current()) const
    {
        if (logif)
            logif->log(level, std::string{loc.function_name()}, msg);
    }

    bool useasync(std::function<void()>&& func)
    {
        if (async.valid())
            async.wait();
        async = std::async(std::launch::async, std::move(func));
        return true;
    };
};

Switch::Switch(const config_t& config) :
    handler{std::make_unique<Handler>(config)}
{}
Switch::~Switch() = default;

bool Switch::observe(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->observe(pin, obs);
}

bool Switch::unobserve(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->unobserve(pin, obs);
}

bool Switch::read(int32_t pin, uint8_t& val)
{
    return handler->read(pin, val);
}

bool Switch::write([[maybe_unused]] int32_t pin, [[maybe_unused]] uint8_t val)
{
    // not supported for switch
    return false;
}

bool Switch::toggle([[maybe_unused]] int32_t pin)
{
    // not supported for switch
    return false;
}

} // namespace gpio::rpi::native::switches
