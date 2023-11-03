#define RYTHE_ENTRY
#define RYTHE_LOG_DEBUG

#if defined(NDEBUG)
    #define RYTHE_KEEP_CONSOLE
#endif
#include <core/core.hpp>
#include <application/application.hpp>
#include <graphics/graphics.hpp>

#include "module/examplemodule.hpp"

#ifdef RYTHE_AUDIO
#include <audio/audio.hpp>
#endif

void RYTHE_CCONV reportModules(rythe::Engine* engine)
{
    using namespace rythe;
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<gfx::RenderingModule>();
    engine->reportModule<ExampleModule>();

#ifdef RYTHE_AUDIO
    engine->reportModule<audio::AudioModule>();
#endif
}
