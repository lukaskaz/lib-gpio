#pragma once

#include "logs/interfaces/logs.hpp"
#include "switch/factory.hpp"
#include "switch/helpers.hpp"
#include "switch/interfaces/switch.hpp"

#include <cstdint>
#include <tuple>
#include <vector>

namespace switcher::rpi::basic
{

enum class modetype
{
    highwhenpressed,
    highwhenreleased
};

using config_t =
    std::tuple<modetype, std::vector<int32_t>, std::shared_ptr<logs::LogIf>>;

class Switch : public SwitchIf
{
  public:
    ~Switch();
    bool observe(
        int32_t,
        std::shared_ptr<switcher::helpers::Observer<SwitchData>>) override;
    bool unobserve(
        int32_t,
        std::shared_ptr<switcher::helpers::Observer<SwitchData>>) override;
    bool read(int32_t, uint8_t&) override;
    bool write(int32_t, uint8_t) override;
    bool toggle(int32_t) override;

  private:
    friend class switcher::Factory;
    explicit Switch(const config_t&);

    struct Handler;
    std::unique_ptr<Handler> handler;
};

} // namespace switcher::rpi::basic
