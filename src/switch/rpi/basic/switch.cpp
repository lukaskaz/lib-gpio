#include "switch/rpi/basic/switch.hpp"

#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>

#include <chrono>
#include <filesystem>
#include <functional>
#include <future>
#include <iomanip>
#include <source_location>
#include <sstream>
#include <unordered_map>

namespace switcher::rpi::basic
{

using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace switcher;
using namespace switcher::helpers;

struct Switch::Handler
{
  public:
    explicit Handler(const config_t& config) :
        logif{std::get<2>(config)}, mode{std::get<0>(config)}
    {
        const auto& pins{std::get<1>(config)};
        std::ranges::for_each(
            pins, [this](auto pin) { switches.try_emplace(pin, this, pin); });

        useasync([this, running = running.get_token()]() {
            try
            {
                log(logs::level::info, "Switches monitoring started");
                while (!running.stop_requested())
                {
                    bool anyobservable{false};
                    std::ranges::for_each(switches,
                                          [this, &anyobservable](auto& sw) {
                                              if (sw.second.monitor())
                                                  anyobservable = true;
                                          });
                    if (!anyobservable)
                        std::this_thread::sleep_for(monitorinterval);
                }
            }
            catch (const std::exception& ex)
            {
                log(logs::level::error, ex.what());
                throw;
            }
        });
    }

    ~Handler()
    {
        running.request_stop();
        switch (mode)
        {
            case modetype::highwhenpressed:
                [[fallthrough]];
            case modetype::highwhenreleased:
                log(logs::level::info, "Switches monitoring ended");
                break;
        }
    }

    bool observe(int32_t switchpin, std::shared_ptr<Observer<SwitchData>> obs)
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

    bool unobserve(int32_t switchpin, std::shared_ptr<Observer<SwitchData>> obs)
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
    class SwitchInput : public Observable<SwitchData>
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
            if (isobserved())
            {
                const auto data = getnextevent();
                switch (std::get<Event>(data))
                {
                    case Event::none:
                        // allow to periodically return from monitoring
                        // loop to notify and check for stop request
                        notifyclients();
                        break;
                    case Event::pressed:
                        state = state->topressed(this, data);
                        break;
                    case Event::released:
                        state = state->toreleased(this, data);
                        break;
                }
                return true;
            }
            return false;
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
            pressed,
            released
        };
        using EventData = std::tuple<Event, uint32_t>;

        EventData getevent(uint32_t gpioevent) const
        {
            static const std::unordered_map<
                modetype, std::unordered_map<uint32_t, EventData>>
                modetoevent{

                    {modetype::highwhenpressed,
                     {{GPIOEVENT_REQUEST_RISING_EDGE,
                       {Event::pressed, GPIOEVENT_REQUEST_FALLING_EDGE}},
                      {GPIOEVENT_REQUEST_FALLING_EDGE,
                       {Event::released, GPIOEVENT_REQUEST_RISING_EDGE}}}},
                    {modetype::highwhenreleased,
                     {{GPIOEVENT_REQUEST_FALLING_EDGE,
                       {Event::pressed, GPIOEVENT_REQUEST_RISING_EDGE}},
                      {GPIOEVENT_REQUEST_RISING_EDGE,
                       {Event::released, GPIOEVENT_REQUEST_FALLING_EDGE}}}}};

            if (modetoevent.contains(handler->mode))
            {
                const auto& gpiotoevent{modetoevent.at(handler->mode)};
                if (gpiotoevent.contains(gpioevent))
                {
                    return gpiotoevent.at(gpioevent);
                }
                handler->log(
                    logs::level::error,
                    "Invalid gpio event: " + std::to_string(gpioevent) +
                        " for switch pin: " + std::to_string(switchpin));
                throw std::runtime_error("Invalid gpio event, details in log");
            }
            handler->log(logs::level::error,
                         "Invalid mode: " +
                             std::to_string(std::underlying_type_t<modetype>(
                                 handler->mode)) +
                             " for switch pin: " + std::to_string(switchpin));
            throw std::runtime_error("Invalid mode, details in log");
        }

        class SwitchStateIf
        {
          public:
            virtual ~SwitchStateIf() = default;
            virtual std::unique_ptr<SwitchStateIf>
                toreleased(SwitchInput* const input,
                           const EventData& eventData) = 0;
            virtual std::unique_ptr<SwitchStateIf>
                topressed(SwitchInput* const input,
                          const EventData& eventData) = 0;
        };

        class PressedState : public SwitchStateIf
        {
          public:
            PressedState(SwitchInput* const input, const EventData& data)
            {
                const auto [event, expectedgpioev] = data;
                input->isreadytonotify = false;
                input->switchedlast = input->getcurrent();
                input->setevent(expectedgpioev);
                input->handler->log(
                    logs::level::debug,
                    "Entered pressed state: waiting for release");
            };
            std::unique_ptr<SwitchStateIf>
                toreleased(SwitchInput* const input,
                           const EventData& data) override
            {
                return std::make_unique<ReleasedState>(input, data);
            }
            std::unique_ptr<SwitchStateIf>
                topressed([[maybe_unused]] SwitchInput* const input,
                          [[maybe_unused]] const EventData& data) override
            {
                throw std::runtime_error(
                    "Invalid state change: PressedState -> topressed");
            }
        };

        class ReleasedState : public SwitchStateIf
        {
          public:
            ReleasedState(SwitchInput* const input, const EventData& data)
            {
                const auto [event, expectedgpioev] = data;
                if (event != Event::none)
                {
                    const auto pressedtime{input->getcurrent() -
                                           input->switchedlast};
                    input->handleevent(pressedtime >= input->longpressdelay);
                    input->isreadytonotify = true;
                }
                input->switchedlast = input->getcurrent();
                input->setevent(expectedgpioev);
                input->handler->log(
                    logs::level::debug,
                    "Entered released state: waiting for press");
            };
            std::unique_ptr<SwitchStateIf>
                topressed(SwitchInput* const input,
                          const EventData& data) override
            {
                return std::make_unique<PressedState>(input, data);
            }

            std::unique_ptr<SwitchStateIf>
                toreleased([[maybe_unused]] SwitchInput* const input,
                           [[maybe_unused]] const EventData& data) override
            {
                throw std::runtime_error(
                    "Invalid state change: ReleasedState -> toreleased");
            }
        };

        const Switch::Handler* handler;
        const int32_t switchpin;
        const std::chrono::milliseconds notifydelay{700ms};
        const std::chrono::milliseconds bouncedelay{50ms};
        const std::chrono::milliseconds longpressdelay{1s};
        const std::chrono::milliseconds eventtimeout{100ms};
        uint32_t longpressnum{};
        uint32_t switchednum{};
        std::chrono::steady_clock::time_point switchedlast{getcurrent()};
        int32_t fd{-1};
        std::unique_ptr<SwitchStateIf> state{std::make_unique<ReleasedState>(
            this, EventData{Event::none, GPIOEVENT_REQUEST_BOTH_EDGES})};
        bool isreadytonotify{false};

        bool initialize()
        {
            while (std::get<Event>(waitforevent(0)) != Event::none)
                ;
            return true;
        }

        bool setevent(uint32_t eventflags)
        {
            if (fd >= 0)
                close(fd);

            struct gpioevent_request req;
            req.lineoffset = switchpin;
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;
            // req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            req.eventflags = eventflags;
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

        EventData getnextevent() const
        {
            EventData temp{}, recent{};
            recent = waitforevent((int32_t)eventtimeout.count());
            std::this_thread::sleep_for(bouncedelay);
            while (std::get<Event>(temp = waitforevent(0)) != Event::none)
                recent = temp;
            return recent;
        }

        EventData waitforevent(int32_t delayms) const
        {
            int epfd = epoll_create1(EPOLL_CLOEXEC);
            if (epfd < 0)
            {
                throw std::runtime_error(
                    "Cannot create epoll instance for pin " +
                    std::to_string(switchpin) +
                    " due to epoll error: " + strerror(errno));
            }

            struct epoll_event event;
            event.events = EPOLLIN | EPOLLET;
            event.data.fd = fd;

            if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) != 0)
            {
                close(epfd);
                throw std::runtime_error(
                    "Cannot register epoll fd for pin " +
                    std::to_string(switchpin) +
                    " due to epoll error: " + strerror(errno));
            }

            struct epoll_event ready;
            const auto epollret = epoll_wait(epfd, &ready, 1, delayms);
            close(epfd);

            if (epollret < 0)
            {
                throw std::runtime_error(
                    "Cannot monitor pin " + std::to_string(switchpin) +
                    " due to epoll error: " + strerror(errno));
            }
            if (epollret == 0)
            {
                return {Event::none, 0};
            }

            handler->log(logs::level::debug, "Got interrupt from switch[" +
                                                 std::to_string(switchpin) +
                                                 "] file descriptor " +
                                                 std::to_string(fd));

            struct gpioevent_data evdata;
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
            handler->log(logs::level::debug,
                         "Read for switch pin[" + std::to_string(switchpin) +
                             "] data size: " + std::to_string(readsize) +
                             " event size: " + std::to_string(sizeof(evdata)));
            if (readsize == sizeof(evdata))
            {
                handler->log(logs::level::debug,
                             "Read for switch pin[" +
                                 std::to_string(switchpin) +
                                 "] id: " + std::to_string(evdata.id));

                return getevent(evdata.id);
            }
            return {Event::none, 0};
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
            isreadytonotify = false;
            return true;
        }

        bool isnotifyneeded() const
        {
            return isreadytonotify && (switchednum || longpressnum) &&
                   getdelay(switchedlast) >= notifydelay;
        }

        bool notifyclients()
        {
            bool ret{};
            if (isnotifyneeded())
            {
                if ((ret = notify(
                         SwitchData{switchpin, switchednum, longpressnum})))
                {
                    handler->log(logs::level::debug,
                                 "Pin[" + std::to_string(switchpin) +
                                     "] clients notified, events num: " +
                                     std::to_string(switchednum) + "/" +
                                     std::to_string(longpressnum));
                }
                else
                {
                    handler->log(logs::level::warning,
                                 "Pin[" + std::to_string(switchpin) +
                                     "] cannot notify clients, events num: " +
                                     std::to_string(switchednum) + "/" +
                                     std::to_string(longpressnum));
                }
                resetevent();
            }
            return ret;
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

bool Switch::observe(int32_t pin, std::shared_ptr<Observer<SwitchData>> obs)
{
    return handler->observe(pin, obs);
}

bool Switch::unobserve(int32_t pin, std::shared_ptr<Observer<SwitchData>> obs)
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

} // namespace switcher::rpi::basic
