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

            auto logif = logs::Factory::create<logs::console::Log>(loglvl);
            std::ranges::for_each(sequence, [logif](
                                                [[maybe_unused]] uint8_t num) {
                using namespace gpio::rpi::wiringpi;
                auto iface = gpio::Factory::create<Gpio, config_t>({"", logif});
            });

            // group->moveto(pospct);
            // std::cout << "Moving to servo position: "
            //           << std::quoted(std::to_string(pospct))
            //           << "[%]\nPress [enter]" << std::flush;
            // getchar();

            // group->movestart();
            // std::cout << "Moving to servo position: " << std::quoted("START")
            //           << "\nPress [enter]" << std::flush;
            // getchar();

            // group->moveend();
            // std::cout << "Moving to servo position: " << std::quoted("END")
            //           << "\nPress [enter]" << std::flush;
            // getchar();

            std::cout << "First scenario DONE -> releasing gpio\n";
        }
    }
    catch (std::exception& err)
    {
        std::cerr << "[ERROR] " << err.what() << '\n';
    }
    return 0;
}
