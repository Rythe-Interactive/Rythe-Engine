#include <core/events/defaultevents.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/logging/logging.hpp>

namespace legion::core::events
{
    exit::exit(int exitCode) : exitcode(exitCode)
    {
        log::undecoratedInfo("=========================\n"
        					 "| Shutting down engine. |\n"
        					 "=========================");
        scheduling::Scheduler::exit(exitCode);
    }
}
