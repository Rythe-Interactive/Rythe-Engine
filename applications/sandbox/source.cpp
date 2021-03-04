#define LEGION_ENTRY
#if defined(NDEBUG)
    #define LEGION_KEEP_CONSOLE
#endif

#include <core/core.hpp>
#include "module/testmodule.hpp"

using namespace legion;

void LEGION_CCONV reportModules(Engine* engine)
{
    log::filter(log::severity::debug);

    engine->reportModule<TestModule>();
}

//int main(void)
//{
//    TestModule tm{};
//    TestSystem ts{};
//
//    std::cout << "test\n";
//    tm.setup();
//    ts.setup();
//    return 0;
//}
