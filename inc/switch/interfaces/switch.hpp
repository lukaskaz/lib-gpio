#pragma once

#include "switch/helpers.hpp"

#include <cstdint>
#include <functional>

namespace switcher
{

using SwitchData = std::tuple<int32_t, int32_t, int32_t>;

class SwitchIf
{
  public:
    virtual ~SwitchIf() = default;
    virtual bool
        observe(int32_t,
                std::shared_ptr<switcher::helpers::Observer<SwitchData>>) = 0;
    virtual bool
        unobserve(int32_t,
                  std::shared_ptr<switcher::helpers::Observer<SwitchData>>) = 0;
    virtual bool read(int32_t, uint8_t&) = 0;
    virtual bool write(int32_t, uint8_t) = 0;
    virtual bool toggle(int32_t) = 0;
};

} // namespace switcher
