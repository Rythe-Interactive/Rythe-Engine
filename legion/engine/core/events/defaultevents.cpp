#include <core/events/defaultevents.hpp>
#include <core/scheduling/scheduler.hpp>
#include <iostream>

namespace legion::core::events
{
    exit::exit(int exitCode) : exitcode(exitCode)
    {
        std::cout << "=========================\n| Shutting down engine. |\n=========================\n";
        scheduling::Scheduler::exit(exitCode);
    }
}
