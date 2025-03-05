#include "gpio/interfaces/rpi/wiringpi/gpio.hpp"

#include <wiringPi.h>

#include <functional>
#include <future>
#include <iostream>
#include <source_location>

namespace gpio::rpi::wpi
{

void isrcall(int32_t pin)
{
    std::cout << "pin: " << pin << std::endl;
}

struct Gpio::Handler : public Observable<GpioData>
{
  public:
    explicit Handler(const config_t& config) :
        logif{std::get<2>(config)}, mode{std::get<0>(config)}, pins{std::get<1>(
                                                                   config)}
    {
        wiringPiSetupGpio();

        if (mode == modetype::input)
        {
            std::ranges::for_each(pins, [](auto pin) { pinMode(pin, INPUT); });
            // wiringPiISR(pins[0], INT_EDGE_FALLING, []() { isrcall(pins[0]);
            // });
            // getchar();
            useasync([this]() {
                while (true)
                {
                    static int32_t prevval{-1};
                    auto pin{pins[0]};
                    auto val = digitalRead(pin);
                    if (prevval != val)
                    {
                        prevval = val;
                        std::cout << "Pin new val: " << val << std::endl;
                        notify({pin, val});
                    }
                    usleep(10 * 1000);
                }
            });
        }
        else
        {
            std::ranges::for_each(pins, [](auto pin) { pinMode(pin, OUTPUT); });
        }
    }

    ~Handler()
    {}

    bool read(uint8_t pin, std::shared_ptr<Observer<GpioData>> obs)
    {
        subscribe(obs);
        return true;
    }

    bool write(uint8_t pin, uint8_t val)
    {
        digitalWrite(pin, val);
        return true;
    }

    bool toggle(uint8_t pin)
    {
        digitalWrite(pin, !digitalRead(pin));
        return true;
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;
    modetype mode;
    std::vector<uint32_t> pins;
    std::future<void> async;

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

bool Gpio::read(uint8_t pin, std::shared_ptr<Observer<GpioData>> obs)
{
    return handler->read(pin, obs);
}

bool Gpio::write(uint8_t pin, uint8_t val)
{
    return handler->write(pin, val);
}

bool Gpio::toggle(uint8_t pin)
{
    return handler->toggle(pin);
}

} // namespace gpio::rpi::wpi
