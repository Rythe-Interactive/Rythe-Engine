#include <core/events/defaultevents.hpp>
#include <iostream>

namespace legion::core::events
{
    exit::exit(int exitcode) :exitcode(exitcode)
    {
        std::cout << "=========================\n| Shutting down engine. |\n=========================\n";
    }
}
