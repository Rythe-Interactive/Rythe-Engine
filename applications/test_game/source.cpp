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

    Records records;

    for (int i = 0; i < 20; i++)
    {
        records.records[i] = MyRecord(i * 10, i * 100, i * 1000);
    }
    serialization::SerializationUtil<Records>::JSONSerialize(std::ofstream("Scene1.cornflake", std::ios::binary), records);

    Records output = serialization::SerializationUtil<Records>::JSONDeserialize(std::ifstream("Scene1.cornflake"));
    std::cout << output.records[10].x;

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
