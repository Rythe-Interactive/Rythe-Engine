#define LEGION_ENTRY
#define LEGION_LOG_DEBUG

#if defined(NDEBUG)
    #define LEGION_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#include <audio/audio.hpp>
#include <physics/physics.hpp>

#include "module/examplemodule.hpp"

void LEGION_CCONV reportModules(legion::Engine* engine)
{
    using namespace legion;
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<gfx::RenderingModule>();
    engine->reportModule<audio::AudioModule>();
    engine->reportModule<physics::PhysicsModule>();
    engine->reportModule<ExampleModule>();
}
