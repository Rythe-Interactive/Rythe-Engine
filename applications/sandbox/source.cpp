#define LEGION_ENTRY
#define LEGION_KEEP_CONSOLE
#define LEGION_LOW_POWER
#define LEGION_MIN_THREADS 6 // Update, Rendering, Input, Audio, Physics, TestChain
//#define LEGION_LOW_POWER

#include <chrono>
#include <thread>

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#define AUDIO_EXIT_ON_FAIL
#include <audio/audio.hpp>

#include "module/testModule.hpp"
#include "systems/testsystem.hpp"

#include <physics/PhysicsModule.hpp>

#include "test_filesystem.hpp"

using namespace legion;

void LEGION_CCONV reportModules(Engine* engine)
{
    log::filter(log::severity::debug);

    //test_filesystem();

    engine->reportModule<TestModule>();
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<rendering::RenderingModule>();
    engine->reportModule<physics::PhysicsModule>();
    engine->reportModule<audio::AudioModule>();
}