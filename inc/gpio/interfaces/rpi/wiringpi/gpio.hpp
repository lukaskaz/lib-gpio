#pragma once

#include "gpio/factory.hpp"
#include "gpio/helpers.hpp"
#include "gpio/interfaces/gpio.hpp"
#include "log/interfaces/console/logs.hpp"

#include <cstdint>
#include <tuple>
#include <vector>

namespace gpio::rpi::wpi
{

enum class modetype
{
    input,
    output,
    tristate
};

using config_t =
    std::tuple<modetype, std::vector<uint32_t>, std::shared_ptr<logs::LogIf>>;

class Gpio : public GpioIf
{
  public:
    ~Gpio();
    bool read(uint8_t, std::shared_ptr<Observer<GpioData>>) override;
    bool write(uint8_t, uint8_t) override;
    bool toggle(uint8_t) override;

  private:
    friend class gpio::Factory;
    explicit Gpio(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace gpio::rpi::wpi
