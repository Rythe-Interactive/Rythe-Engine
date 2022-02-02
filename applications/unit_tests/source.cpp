#define LEGION_LOG_WARN

#include <core/core.hpp>

#define DOCTEST_CONFIG_IMPLEMENT

#include "doctest.h"
#include "tests/core/filesystem.hpp"
#include "tests/core/ecs.hpp"
#include "tests/core/containers/delegate.hpp"

int main(int argc, char** argv)
{
    using namespace legion::core;
    log::setup();

    enterRealtimePriority();

    doctest::Context ctx;
    ctx.applyCommandLine(argc, argv);

    return ctx.run();
}
