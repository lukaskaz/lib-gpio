#include "gpio/interfaces/rpi/wiringpi/gpio.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

int main(int argc, char** argv)
{
    try
    {
        if (argc >= 5)
        {
            std::cout << "First scenario -> testing gpio opeartion\n";
            const auto number = (int32_t)atoi(argv[1]);
            const auto mode = (std::string)argv[2];
            [[maybe_unused]] const auto value = (uint8_t)atoi(argv[3]);
            auto loglvl =
                (bool)atoi(argv[4]) ? logs::level::debug : logs::level::info;
            auto sequence = std::views::iota(0, number);

            using namespace gpio::rpi::wpi;
            auto logif = logs::Factory::create<logs::console::Log>(loglvl);
            auto iface = gpio::Factory::create<Gpio, config_t>(
                {modetype::input, {16, 21}, logif});

            auto readingfunc = Observer<gpio::GpioData>::create(
                [](const gpio::GpioData& data) {
                    std::cout << "Pin: " << std::get<0>(data)
                              << ", val: " << std::get<1>(data) << std::endl;
                });
            iface->read(16, readingfunc);
            iface->read(21, readingfunc);
            getchar();

            iface->unread(16, readingfunc);
            std::cout << "First scenario DONE -> releasing gpio\n";
        }
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
