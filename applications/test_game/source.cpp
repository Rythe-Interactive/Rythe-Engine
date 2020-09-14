#define ARGS_ENTRY
#define ARGS_KEEP_CONSOLE

#include <chrono>
#include <thread>

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>

#include "module/testModule.hpp"
#include "systems/testsystem.hpp"

#include <physics/PhysicsModule.hpp>

#include "test_filesystem.hpp"

using namespace args;



void ARGS_CCONV reportModules(Engine* engine)
{
    log::info("Hello Args!");

    test_filesystem();

    engine->reportModule<TestModule>();
    engine->reportModule<app::ApplicationModule>();
    engine->reportModule<rendering::RenderingModule>();
    engine->reportModule<physics::PhysicsModule>();

    MyRecord record;
    record.x = 10;
    record.y = 100;
    record.z = 10000;
    serialization::SerializationUtil<MyRecord>::serialize(std::ofstream("Scene1.cornflake", std::ios::binary), record);

    MyRecord output = serialization::SerializationUtil<MyRecord>::deserialize(std::ifstream("Scene1.cornflake"));
    std::cout << output.x;

    try
    {
        throw args_component_destroyed_error;
    }
    catch (const exception& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << e.file() << std::endl;
        std::cout << e.line() << std::endl;
        std::cout << e.func() << std::endl;
    }
}
