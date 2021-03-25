#define LEGION_ENTRY
#define LEGION_SHOW_CONSOLE

#include <core/core.hpp>

#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include "tests/core/filesystem.hpp"
#include "tests/core/ecs.hpp"
#include "tests/core/containers/delegate.hpp"

using namespace legion;

class Exitus : public Module {
public:
    void setup() override
    {
        reportSystem<ExitHelper>();
    }

    priority_type priority() override { return PRIORITY_MAX; };

    class ExitHelper : public System<ExitHelper>
    {
    public:
        void setup()
        {
            //raiseEvent<events::exit>();
        }
    };
};

void LEGION_CCONV reportModules(Engine* engine)
{
    doctest::Context ctx;
    ctx.applyCommandLine(static_cast<int>(engine->cliargs.size()), engine->cliargs.data());

    engine->exitCode = ctx.run();

    if (ctx.shouldExit())
        engine->reportModule<Exitus>();

    schd::Scheduler::exit(0);
    //std::exit(res);

// additional application code
}
