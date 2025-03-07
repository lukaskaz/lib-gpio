#pragma once

#include "gpio/factory.hpp"
#include "gpio/helpers.hpp"
#include "gpio/interfaces/gpio.hpp"
#include "logs/interfaces/console/logs.hpp"

#include <cstdint>
#include <tuple>
#include <vector>

namespace gpio::rpi::native
{

enum class modetype
{
    input,
    output,
    tristate
};

using config_t =
    std::tuple<modetype, std::vector<int32_t>, std::shared_ptr<logs::LogIf>>;

class Gpio : public GpioIf
{
  public:
    ~Gpio();
    bool read(int32_t, std::shared_ptr<Observer<GpioData>>) override;
    bool unread(int32_t, std::shared_ptr<Observer<GpioData>>) override;
    bool write(int32_t, uint8_t) override;
    bool toggle(int32_t) override;

  private:
    friend class gpio::Factory;
    explicit Gpio(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace gpio::rpi::native
