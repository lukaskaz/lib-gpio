#pragma once

#include "switch/interfaces/switch.hpp"

#include <memory>

namespace switcher
{

class Factory
{
  public:
    template <typename T, typename C>
    static std::shared_ptr<SwitchIf> create(const C& config)
    {
        return std::shared_ptr<T>(new T(config));
    }
};

} // namespace switcher
