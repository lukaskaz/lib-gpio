#include "gpio/interfaces/rpi/wiringpi/gpio.hpp"

#include <functional>
#include <future>
#include <source_location>

namespace gpio::rpi::wiringpi
{

struct Gpio::Handler
{
  public:
    explicit Handler(const config_t& config)
    {}

    ~Handler()
    {}

    bool read(uint8_t)
    {
        return true;
    }

    bool write(uint8_t&)
    {
        return true;
    }

    bool toggle()
    {
        return true;
    }

  private:
    const std::shared_ptr<logs::LogIf> logif;

    void log(
        logs::level level, const std::string& msg,
        const std::source_location loc = std::source_location::current()) const
    {
        if (logif)
            logif->log(level, std::string{loc.function_name()}, msg);
    }
};

Gpio::Gpio(const config_t& config) : handler{std::make_unique<Handler>(config)}
{}
Gpio::~Gpio() = default;

bool Gpio::read(uint8_t& val)
{
    return handler->read(val);
}

bool Gpio::write(uint8_t val)
{
    return handler->write(val);
}

bool Gpio::toggle()
{
    return handler->toggle();
}

} // namespace gpio::rpi::wiringpi
