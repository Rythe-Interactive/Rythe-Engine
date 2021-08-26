#include <core/events/defaultevents.hpp>
#include <core/scheduling/scheduler.hpp>
#include <core/logging/logging.hpp>

namespace legion::core::events
{
    exit::exit(int exitCode) : exitcode(exitCode)
    {
    }
}
