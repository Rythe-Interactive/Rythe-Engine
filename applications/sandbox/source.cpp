#define LEGION_ENTRY
#if defined(NDEBUG)
#define LEGION_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <application/application.hpp>

#include "module/examplemodule.hpp"

void LEGION_CCONV reportModules(legion::Engine* engine)
{
    engine->reportModule<lgn::app::ApplicationModule>();
    engine->reportModule<ExampleModule>();
}
