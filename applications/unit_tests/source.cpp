#define ARGS_ENTRY
#define ARGS_KEEP_CONSOLE
#define ARGS_LOW_POWER

#include <core/core.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>


#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include "test_filesystem.hpp"

using namespace args;

void ARGS_CCONV reportModules(Engine* engine)
{
    doctest::Context ctx;
    ctx.applyCommandLine(engine->getCliArgs().size(),engine->getCliArgs().data());

    const int res = ctx.run();

    if(ctx.shouldExit())
        std::exit(res);

    // additional application code
}
