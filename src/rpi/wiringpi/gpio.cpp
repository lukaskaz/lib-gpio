#include "gpio/interfaces/rpi/wiringpi/gpio.hpp"

#include <asm/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <poll.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
// #include <wiringPi.h> // torem

#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <unordered_map>

namespace gpio::rpi::wpi
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
                            usleep(10 * 1000);
                        }
                    }
                    catch (const std::exception& ex)
                    {
                        log(logs::level::error, ex.what());
                    }
                });
                break;
            case modetype::output:
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
            case modetype::output:
                log(logs::level::info, "Output pins operation ended");
            case modetype::tristate:
                // yet not supported
                break;
        }
    }

    bool read(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
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

    bool unread(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
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
            handler->log(logs::level::info,
                         "Removed input pin: " + std::to_string(pin));
        }

        bool monitor()
        {
            if (switchingnum > 0)
            {
                if (getdelay(switchedlast) < bouncedelay)
                {
                    while (getpendingevent())
                        ;
                    return true;
                }
                if (getdelay(switchedlast) >= signaldelay)
                    return notify();
            }

            if (getpendingevent() == GPIOEVENT_EVENT_FALLING_EDGE)
            {
                switchedlast = getcurrent();
                switchingnum++;
            }

            return true;
        }

      public:
        const Gpio::Handler* handler;
        const int32_t pin;
        const std::chrono::milliseconds signaldelay{500ms};
        const std::chrono::milliseconds bouncedelay{200ms};
        int32_t switchingnum{};
        std::chrono::steady_clock::time_point switchedlast{};
        int32_t fd;

        bool initialize()
        {
            struct gpioevent_request req;
            req.lineoffset = pin;
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;
            // req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            req.handleflags =
                GPIOHANDLE_REQUEST_INPUT | GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
            // switch (mode)
            // {
            //     case INT_EDGE_SETUP:
            //         return -1;
            //     case INT_EDGE_FALLING:
            //         req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            //         break;
            //     case INT_EDGE_RISING:
            //         req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
            //         break;
            //     case INT_EDGE_BOTH:
            //         req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            //         break;
            // }

            // Chip 0 on older Pi models, chip 4 on Pi 5
            std::filesystem::path gpiochippath{"/dev/gpiochip4"};
            auto ifs = fopen(gpiochippath.c_str(), "r");
            if (!ifs)
                throw std::runtime_error(
                    "Cannot open gpio chip file for input: " +
                    gpiochippath.native());

            if (ioctl(fileno(ifs), GPIO_GET_LINEEVENT_IOCTL, &req) != 0)
                throw std::runtime_error(
                    "Cannot initialize pin " + std::to_string(pin) +
                    " due to ioctl error: " + strerror(errno));

            fd = req.fd;
            handler->log(logs::level::debug,
                         "Obtained input pin[" + std::to_string(pin) +
                             "] file descriptor " + std::to_string(fd));

            auto flags = fcntl(fd, F_GETFL);
            flags |= O_NONBLOCK;
            if (fcntl(fd, F_SETFL, flags) < 0)
                throw std::runtime_error(
                    "Cannot initialize pin " + std::to_string(pin) +
                    " due to fcntl error: " + strerror(errno));

            return true;
        }

        int32_t getpendingevent() const
        {
            return waitforevent(0);
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

        bool notify()
        {
            if (Observable<GpioData>::notify({pin, switchingnum}))
                handler->log(logs::level::debug,
                             "Pin[" + std::to_string(pin) +
                                 "] clients notified, events num: " +
                                 std::to_string(switchingnum));
            else
                handler->log(logs::level::warning,
                             "Pin[" + std::to_string(pin) +
                                 "] cannot notify clients, events num: " +
                                 std::to_string(switchingnum));
            switchingnum = 0;
            return true;
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
            handler{handler}, pin{pin}
        {
            // pinMode(pin, OUTPUT);
            handler->log(logs::level::info,
                         "Created output pin: " + std::to_string(pin));
        }
        ~OutputPin()
        {
            // pinMode(pin, OUTPUT);
            handler->log(logs::level::info,
                         "Removed output pin: " + std::to_string(pin));
        }

        bool set()
        {
            // digitalWrite(pin, 1);
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] set");
            return true;
        }

        bool clear()
        {
            // digitalWrite(pin, 0);
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] cleared");
            return true;
        }

        bool toggle()
        {
            // digitalWrite(pin, !digitalRead(pin));
            handler->log(logs::level::debug,
                         "Pin[" + std::to_string(pin) + "] toggled");
            return true;
        }

      private:
        const Gpio::Handler* handler;
        const int32_t pin;
    };
    std::unordered_map<int32_t, InputPin> inputs;
    std::unordered_map<int32_t, OutputPin> outputs;
    std::future<void> async;
    std::stop_source running;

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

bool Gpio::read(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->read(pin, obs);
}

bool Gpio::unread(int32_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->unread(pin, obs);
}

bool Gpio::write(int32_t pin, uint8_t val)
{
    return handler->write(pin, val);
}

bool Gpio::toggle(int32_t pin)
{
    return handler->toggle(pin);
}

} // namespace gpio::rpi::wpi
