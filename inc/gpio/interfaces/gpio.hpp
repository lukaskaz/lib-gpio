#pragma once

#include <cstdint>

namespace gpio
{

class GpioIf
{
  public:
    virtual ~GpioIf() = default;
    virtual bool read(uint8_t&) = 0;
    virtual bool write(uint8_t) = 0;
    virtual bool toggle() = 0;
};

} // namespace gpio
