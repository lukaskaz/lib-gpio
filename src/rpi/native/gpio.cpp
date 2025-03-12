#include "gpio/interfaces/rpi/native/gpio.hpp"

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

namespace gpio::rpi::native
{

using namespace std::chrono_literals;
using namespace std::string_literals;

struct Gpio::Handler
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
                    inputs.try_emplace(pin, this, pin);
                });

                useasync([this, running = running.get_token()]() {
                    try
                    {
                        log(logs::level::info, "Input pins monitoring started");
                        while (!running.stop_requested())
                        {
                            std::ranges::for_each(inputs, [this](auto& pin) {
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
            case modetype::output_normal:
                [[fallthrough]];
            case modetype::output_inverted:
                std::ranges::for_each(pins, [this](auto pin) {
                    outputs.try_emplace(pin, this, pin);
                });
                log(logs::level::info, "Output pins operation started");
                break;
            case modetype::tristate:
                throw std::runtime_error("Mode for given pins not supported" +
                                         std::to_string((int32_t)mode));
        }
    }

    ~Handler()
    {
        running.request_stop();
        switch (mode)
        {
            case modetype::input:
                log(logs::level::info, "Input pins operation ended");
                break;
            case modetype::output_normal:
                [[fallthrough]];
            case modetype::output_inverted:
                log(logs::level::info, "Output pins operation ended");
            case modetype::tristate:
                // yet not supported
                break;
        }
    }

    bool observe(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
    {
        if (inputs.contains(pin))
        {
            std::ostringstream oss;
            oss << std::hex << obs.get();
            log(logs::level::debug, "Adding observer "s + oss.str() +
                                        " for pin " + std::to_string(pin));
            inputs.at(pin).subscribe(obs);
            return true;
        }
        return false;
    }

    bool unobserve(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
    {
        if (inputs.contains(pin))
        {
            std::ostringstream oss;
            oss << std::hex << obs.get();
            log(logs::level::debug, "Removing observer "s + oss.str() +
                                        " for pin " + std::to_string(pin));
            inputs.at(pin).unsubscribe(obs);
            return true;
        }
        return false;
    }

    bool read(int32_t pin, uint8_t& val)
    {
        if (inputs.contains(pin))
        {
            auto& in{inputs.at(pin)};
            val = in.read();
            return true;
        }
        return false;
    }

    bool write(int32_t pin, uint8_t val)
    {
        if (outputs.contains(pin))
        {
            auto& out{outputs.at(pin)};
            return val == 0 ? out.clear() : out.set();
        }
        return false;
    }

    bool toggle(int32_t pin)
    {
        if (outputs.contains(pin))
            return outputs.at(pin).toggle();
        return false;
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;
    // Chip 0 on older Pi models, chip 4 on Pi 5
    const std::filesystem::path gpiochippath{"/dev/gpiochip4"};
    const modetype mode;
    class InputPin : public Observable<GpioData>
    {
      public:
        InputPin(Gpio::Handler* handler, int32_t pin) :
            handler{handler}, pin{pin}
        {
            initialize();
            handler->log(logs::level::info,
                         "Created input pin: " + std::to_string(pin));
        }
        ~InputPin()
        {
            deinitialize();
            handler->log(logs::level::info,
                         "Removed input pin: " + std::to_string(pin));
        }

        bool monitor()
        {
            if (getpendingevent() == expectedevent)
            {
                if (getdelay(switchedlast) >= bouncedelay)
                    return handleevent();
            }
            else
            {
                if (isnotifyneeded() && getdelay(switchedlast) >= notifydelay)
                    return notifyclients();
            }
            return false;
        }

        uint8_t read() const
        {
            struct gpiohandle_data data;
            if (ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) != 0)
                throw std::runtime_error(
                    "Cannot get input pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));
            return data.values[0];
        }

      private:
        const Gpio::Handler* handler;
        const int32_t pin;
        const std::chrono::milliseconds notifydelay{500ms};
        const std::chrono::milliseconds bouncedelay{200ms};
        const int32_t expectedevent{GPIOEVENT_REQUEST_FALLING_EDGE};
        uint32_t switchednum{};
        std::chrono::steady_clock::time_point switchedlast{};
        int32_t fd;

        bool initialize()
        {
            struct gpioevent_request req;
            req.lineoffset = pin;
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;
            // req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            // req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
            // req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            req.handleflags =
                GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
            strcpy(req.consumer_label, "rpi_input");

            auto ifs = fopen(handler->gpiochippath.c_str(), "r");
            if (!ifs)
                throw std::runtime_error(
                    "Cannot open gpio chip file for input pin: " +
                    handler->gpiochippath.native());

            if (ioctl(fileno(ifs), GPIO_GET_LINEEVENT_IOCTL, &req) != 0)
                throw std::runtime_error(
                    "Cannot initialize input pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));

            fclose(ifs);
            fd = req.fd;
            handler->log(logs::level::debug,
                         "Obtained input pin[" + std::to_string(pin) +
                             "] file descriptor " + std::to_string(fd));

            auto flags = fcntl(fd, F_GETFL);
            flags |= O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
                throw std::runtime_error(
                    "Cannot initialize input pin " + std::to_string(pin) +
                    " due to fcntl error: " + strerror(errno));

            return true;
        }

        bool deinitialize()
        {
            close(fd);
            return true;
        }

        int32_t getpendingevent() const
        {
            int32_t lastevent{};
            while (true)
                if (auto ret = waitforevent(0); ret > 0)
                    lastevent = ret;
                else
                    break;
            return lastevent;
        }

        int32_t waitforevent(int32_t delayms) const
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
                    "Cannot monitor pin " + std::to_string(pin) +
                    " due to poll error: " + strerror(errno));
            }
            else if (pollret > 0)
            {
                handler->log(logs::level::debug,
                             "Got interrupt from pin[" + std::to_string(pin) +
                                 "] file descriptor " + std::to_string(fd));

                auto readsize = ::read(fd, &evdata, sizeof(evdata));
                if (readsize < 0)
                {
                    int32_t readretry{10};
                    while (--readretry &&
                           (readsize = ::read(fd, &evdata, sizeof(evdata))) < 0)
                        ;
                    if (!readretry)
                        throw std::runtime_error(
                            "Cannot monitor pin " + std::to_string(pin) +
                            " due to read error (" + std::to_string(readsize) +
                            "): " + strerror(errno));
                }
                handler->log(
                    logs::level::debug,
                    "Read for pin[" + std::to_string(pin) +
                        "] data size: " + std::to_string(readsize) +
                        " event size: " + std::to_string(sizeof(evdata)));
                if (readsize == sizeof(evdata))
                {
                    handler->log(logs::level::debug,
                                 "Read for pin[" + std::to_string(pin) +
                                     "] id: " + std::to_string(evdata.id));
                    return evdata.id;
                }
            }
            else
            {
                // pollret == 0, no event detected
            }
            return 0;
        }

        bool handleevent()
        {
            switchedlast = getcurrent();
            switchednum++;
            return true;
        }

        bool resetevent()
        {
            switchednum = 0;
            return true;
        }

        bool isnotifyneeded() const
        {
            return switchednum;
        }

        bool notifyclients()
        {
            bool ret{};
            if ((ret = notify({pin, switchednum})))
                handler->log(logs::level::debug,
                             "Pin[" + std::to_string(pin) +
                                 "] clients notified, events num: " +
                                 std::to_string(switchednum));
            else
                handler->log(logs::level::warning,
                             "Pin[" + std::to_string(pin) +
                                 "] cannot notify clients, events num: " +
                                 std::to_string(switchednum));
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
        }
    };

    class OutputPin
    {
      public:
        OutputPin(Gpio::Handler* handler, int32_t pin) :
            handler{handler}, low{[this] {
                return (uint8_t)(isnormal() ? 0 : 1);
            }()},
            high{!low}, pin{pin}
        {
            initialize();
            handler->log(logs::level::info,
                         "Created output pin: " + std::to_string(pin) +
                             ", type: " + (isnormal() ? "normal" : "inverted"));
        }
        ~OutputPin()
        {
            deinitialize();
            handler->log(logs::level::info,
                         "Removed output pin: " + std::to_string(pin) +
                             ", type: " + (isnormal() ? "normal" : "inverted"));
        }

        bool set() const
        {
            write(high);
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] set high");
            return true;
        }

        bool clear() const
        {
            write(low);
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] set low");
            return true;
        }

        bool toggle() const
        {
            write(!read());
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] toggled");
            return true;
        }

      private:
        const Gpio::Handler* handler;
        const uint8_t low;
        const uint8_t high;
        const int32_t pin;
        int32_t fd;

        bool initialize()
        {
            struct gpiohandle_request req;
            req.lineoffsets[0] = pin;
            req.flags =
                GPIOHANDLE_REQUEST_OUTPUT | GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
            req.lines = 1;
            req.default_values[0] = low;
            strcpy(req.consumer_label, "rpi_output");

            auto ifs = fopen(handler->gpiochippath.c_str(), "r");
            if (!ifs)
                throw std::runtime_error(
                    "Cannot open gpio chip file for output pin: " +
                    handler->gpiochippath.native());

            if (ioctl(fileno(ifs), GPIO_GET_LINEHANDLE_IOCTL, &req) != 0)
                throw std::runtime_error(
                    "Cannot initialize output pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));

            fclose(ifs);
            fd = req.fd;
            handler->log(logs::level::debug,
                         "Obtained output pin[" + std::to_string(pin) +
                             "] file descriptor " + std::to_string(fd));

            auto flags = fcntl(fd, F_GETFL);
            flags |= O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
                throw std::runtime_error(
                    "Cannot initialize output pin " + std::to_string(pin) +
                    " due to fcntl error: " + strerror(errno));

            return true;
        }

        bool deinitialize()
        {
            clear();
            close(fd);
            return true;
        }

        bool write(uint8_t val) const
        {
            struct gpiohandle_data data;
            data.values[0] = val;
            if (ioctl(fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data) != 0)
                throw std::runtime_error(
                    "Cannot set output pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) +
                             "] written to: " + std::to_string(val));
            return true;
        }

        uint8_t read() const
        {
            struct gpiohandle_data data;
            if (ioctl(fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data) != 0)
                throw std::runtime_error(
                    "Cannot get output pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));
            return data.values[0];
        }

        bool isnormal() const
        {
            return handler->mode == modetype::output_normal;
        }

        bool isinverted() const
        {
            return handler->mode == modetype::output_inverted;
        }
    };
    std::unordered_map<int32_t, InputPin> inputs;
    std::unordered_map<int32_t, OutputPin> outputs;
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

Gpio::Gpio(const config_t& config) : handler{std::make_unique<Handler>(config)}
{}
Gpio::~Gpio() = default;

bool Gpio::observe(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->observe(pin, obs);
}

bool Gpio::unobserve(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->unobserve(pin, obs);
}

bool Gpio::read(int32_t pin, uint8_t& val)
{
    return handler->read(pin, val);
}

bool Gpio::write(int32_t pin, uint8_t val)
{
    return handler->write(pin, val);
}

bool Gpio::toggle(int32_t pin)
{
    return handler->toggle(pin);
}

} // namespace gpio::rpi::native
