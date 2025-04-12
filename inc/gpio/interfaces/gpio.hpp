#pragma once

#include "gpio/helpers.hpp"

#include <cstdint>
#include <functional>

namespace gpio
{

using GpioData = std::pair<int32_t, int32_t>;

class GpioIf
{
  public:
    virtual ~GpioIf() = default;
    virtual bool
        observe(int32_t,
                std::shared_ptr<gpio::helpers::Observer<GpioData>>) = 0;
    virtual bool
        unobserve(int32_t,
                  std::shared_ptr<gpio::helpers::Observer<GpioData>>) = 0;
    virtual bool read(int32_t, uint8_t&) = 0;
    virtual bool write(int32_t, uint8_t) = 0;
    virtual bool toggle(int32_t) = 0;
};

} // namespace gpio
