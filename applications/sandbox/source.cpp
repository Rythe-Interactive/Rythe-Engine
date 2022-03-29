#define LEGION_ENTRY
#define LEGION_LOG_DEBUG

#if defined(NDEBUG)
    #define LEGION_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include <application/application.hpp>
#include <graphics/graphics.hpp>
#include "module/examplemodule.hpp"

#ifdef RYTHE_AUDIO
#include <audio/audio.hpp>
#endif

void LEGION_CCONV reportModules(legion::Engine* engine)
{
    using namespace legion;
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<gfx::RenderingModule>();
    engine->reportModule<ExampleModule>();

#ifdef RYTHE_AUDIO
    engine->reportModule<audio::AudioModule>();
#endif
}
