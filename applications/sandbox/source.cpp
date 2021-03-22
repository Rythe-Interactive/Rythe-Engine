#define LEGION_ENTRY
#if defined(NDEBUG)
    #define LEGION_KEEP_CONSOLE
#endif
//#define LEGION_LOW_POWER
#define LEGION_MIN_THREADS 4 // Update, Rendering, Input, Physics
#define AUDIO_EXIT_ON_FAIL

#include "module/testmodule.hpp"

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#include <audio/audio.hpp>
#include <physics/physics.hpp>
#include <rendering/module/renderingmodule.hpp>

using namespace legion;

void LEGION_CCONV reportModules(Engine* engine)
{
    log::filter(log::severity::debug);

    engine->reportModule<TestModule>();
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<rendering::RenderingModule>();
    engine->reportModule<physics::PhysicsModule>();
    engine->reportModule<audio::AudioModule>();
}
