#pragma once

#include "gpio/factory.hpp"
#include "gpio/interfaces/gpio.hpp"
#include "log/interfaces/console/logs.hpp"

#include <cstdint>
#include <tuple>

namespace gpio::rpi::wiringpi
{

using config_t = std::tuple<std::string, std::shared_ptr<logs::LogIf>>;

class Gpio : public GpioIf
{
  public:
    ~Gpio();
    bool read(uint8_t&) override;
    bool write(uint8_t) override;
    bool toggle() override;

  private:
    friend class gpio::Factory;
    explicit Gpio(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace gpio::rpi::wiringpi
