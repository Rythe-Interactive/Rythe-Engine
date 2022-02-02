#pragma once
#include <core/engine/engine.hpp>
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>
#include <iostream>

/**
 * @file entry_point.hpp
 * @brief When LEGION_ENTRY is defined, this file will create a function with signature main(int,char**) -> int
 *        implementing the common main function of a c++ program.
 * @note When defining LEGION_ENTRY do no create your own CRT_STARTUP such as main()->int, main(int,char**)->int, wmain(), etc...
 * @note When using LEGION_ENTRY you must instead implement reportModules(legion::core::Engine*).
 * @note When not using LEGION_ENTRY you must call creation and initialization of the engine manually.
 */


 /**@brief Reports engine modules to the engine, must be implemented by you.
  * @param [in] engine The engine object as ptr *
  * @ref legion::core::Engine::reportModule<T,...>()
  */
extern void reportModules(legion::core::Engine* engine);

namespace legion::core
{
    static void enterRealtimePriority()
    {
#if defined(LEGION_WINDOWS)
        if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
        {
            DWORD error = GetLastError();
            log::undecoratedInfo(
                "==============================================================\n"
                "| Failed to enter real-time performance mode, error: {} |\n"
                "==============================================================", error);
            return;
        }
#elif defined(LEGION_LINUX)
        pid_t pid = getpid();
        if (setpriority(PRIO_PROCESS, pid, sched_get_priority_max(sched_getscheduler(pid))) == -1)
        {
            int errornum = errno;
            std::string error;

            switch (errornum)
            {
            case ESRCH:
                error = "ESRCH";
                break;
            case EINVAL:
                error = "EINVAL";
                break;
            case EPERM:
                error = "EPERM";
                break;
            case EACCES:
                error = "EACCES";
                break;
            default:
                error = std::to_string(errornum);
                break;
            }

            log::undecoratedInfo(
                "=============================================================\n"
                "| Failed to enter real-time performance mode, error: {} |\n"
                "=============================================================", error);
            return;
        }
#endif
        log::undecoratedInfo(
            "=======================================\n"
            "| Entered real-time performance mode. |\n"
            "=======================================");
    }
}

#if defined(LEGION_ENTRY)

#if (defined(LEGION_HIGH_PERFORMANCE) && defined(LEGION_WINDOWS))
__declspec(dllexport) DWORD NvOptimusEnablement = 0x0000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

int main(int argc, char** argv)
{
    using namespace legion::core;

#if defined(LEGION_WINDOWS) && (!(defined(LEGION_KEEP_CONSOLE) || defined(LEGION_SHOW_CONSOLE) || defined(LEGION_DEBUG)) || defined(LEGION_HIDE_CONSOLE))
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

    log::setup();

#if defined(LEGION_HIGH_PERFORMANCE)
    enterRealtimePriority();
#else
    log::undecoratedInfo(
        "========================================\n"
        "| Engine will start in low power mode. |\n"
        "========================================");
#endif

    Engine engine{ argc, argv };

    reportModules(&engine);

#if defined(LEGION_LOW_POWER)
    engine.run(true, LEGION_MIN_THREADS);
#else
    engine.run(false, LEGION_MIN_THREADS);
#endif

#if defined(LEGION_KEEP_CONSOLE)
    log::undecoratedInfo("Press enter to exit.");
    std::cin.ignore();
#endif
    return engine.exitCode;
}
#endif
