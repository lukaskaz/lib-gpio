#pragma once

#include "gpio/interfaces/gpio.hpp"

#include <memory>

namespace gpio
{

class Factory
{
  public:
    template <typename T, typename C>
    static std::shared_ptr<GpioIf> create(const C& config)
    {
        return std::shared_ptr<T>(new T(config));
    }
};

} // namespace gpio
