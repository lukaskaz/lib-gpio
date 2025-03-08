#include "gpio/interfaces/rpi/native/gpio.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <vector>

int main(int argc, char** argv)
{
    try
    {
        if (argc >= 2)
        {
            auto loglvl =
                (bool)atoi(argv[1]) ? logs::level::debug : logs::level::info;

            using namespace gpio::rpi::native;
            auto logif = logs::Factory::create<logs::console::Log>(loglvl);
            {
                std::cout << "First scenario -> testing gpio input operation\n";
                auto ifaceread = gpio::Factory::create<Gpio, config_t>(
                    {modetype::input, {16, 21}, logif});

                auto readingfunc = Observer<gpio::GpioData>::create(
                    [](const gpio::GpioData& data) {
                        std::cout << "Pin: " << std::get<0>(data)
                                  << ", val: " << std::get<1>(data)
                                  << std::endl;
                    });
                ifaceread->read(16, readingfunc);
                ifaceread->read(21, readingfunc);
                getchar();
                ifaceread->unread(16, readingfunc);
                std::cout << "First scenario DONE -> releasing gpio\n";
            }

            {
                std::cout << "Second scenario -> testing gpio normal output "
                             "operation\n";
                auto ifacewrite = gpio::Factory::create<Gpio, config_t>(
                    {modetype::output_normal, {17, 22}, logif});

                ifacewrite->write(17, 1);
                ifacewrite->write(22, 0);
                getchar();
                ifacewrite->write(17, 0);
                ifacewrite->write(22, 1);
                getchar();
                ifacewrite->toggle(17);
                ifacewrite->toggle(22);
                getchar();
                ifacewrite->toggle(17);
                ifacewrite->toggle(22);
                getchar();

                std::cout << "Second scenario DONE -> releasing gpio\n";
            }

            {
                std::cout << "Third scenario -> testing gpio inverted output "
                             "operation\n";
                auto ifacewrite = gpio::Factory::create<Gpio, config_t>(
                    {modetype::output_inverted, {17, 22}, logif});

                ifacewrite->write(17, 1);
                ifacewrite->write(22, 0);
                getchar();
                ifacewrite->write(17, 0);
                ifacewrite->write(22, 1);
                getchar();
                ifacewrite->toggle(17);
                ifacewrite->toggle(22);
                getchar();
                ifacewrite->toggle(17);
                ifacewrite->toggle(22);
                getchar();

                std::cout << "Third scenario DONE -> releasing gpio\n";
            }
        }
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
