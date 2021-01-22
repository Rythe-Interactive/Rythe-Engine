#pragma once
#include <core/platform/platform.hpp>
#include <thread>

namespace legion::core::async
{
#ifdef LEGION_WINDOWS
    const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)


    inline void set_thread_name(uint32_t dwThreadID, const char* threadName)
    {

        // DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>( t.native_handle() ) );

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = threadName;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }

    inline void set_thread_name(const char* threadName)
    {
        set_thread_name(GetCurrentThreadId(), threadName);
    }

    inline void set_thread_name(std::thread& thread, const char* threadName)
    {
        DWORD threadId = ::GetThreadId(static_cast<HANDLE>(thread.native_handle()));
        set_thread_name(threadId, threadName);
    }

#elif defined(LEGION_LINUX)
    inline void set_thread_name(const char* threadName)
    {
        prctl(PR_SET_NAME, threadName, 0, 0, 0);
    }

    inline void set_thread_name(std::thread& thread, const char* threadName)
    {
        auto handle = thread.native_handle();
        pthread_setname_np(handle, threadName);
    }
#endif
}
