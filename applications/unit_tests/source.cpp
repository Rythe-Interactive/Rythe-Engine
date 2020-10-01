#define ARGS_ENTRY
#define ARGS_KEEP_CONSOLE
#define ARGS_LOW_POWER
#define ARGS_ON_EXIT_QUIT

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>


#define DOCTEST_CONFIG_IMPLEMENT



#include "doctest.h"
#include "test_filesystem.hpp"

using namespace args;

class Exitus : public Module {
public:
    void setup() override
    {
        reportSystem<ExitHelper>();
    }

    priority_type priority() override { return PRIORITY_MAX ;};

    class ExitHelper : public System<ExitHelper>
    {
    public:
        void setup() override
        {
            raiseEvent<events::exit>();
        }
    };
};

TEST_CASE("[core:ut] sanity-check")
{
    CHECK_EQ(true,true);
}


void ARGS_CCONV reportModules(Engine* engine)
{
    doctest::Context ctx;
    ctx.applyCommandLine(engine->getCliArgs().size(),engine->getCliArgs().data());

    const int res = ctx.run();

    if(ctx.shouldExit())
        engine->reportModule<Exitus>();
        //std::exit(res);

    // additional application code
}
