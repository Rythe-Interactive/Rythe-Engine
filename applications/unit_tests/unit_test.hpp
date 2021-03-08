#pragma once
#include <core/core.hpp>
#include "doctest.h"

namespace legion
{
    template <class T>
    inline L_ALWAYS_INLINE void DoNotOptimize(const T& value)
    {
        asm volatile("" : : "r,m"(value) : "memory");
    }

    template <class T>
    inline L_ALWAYS_INLINE void DoNotOptimize(T& value)
    {
#if defined(LEGION_CLANG)
        asm volatile("" : "+r,m"(value) : : "memory");
#else
        asm volatile("" : "+m,r"(value) : : "memory");
#endif
    }

    using CheckFunc = void(*)(bool);

    CheckFunc Check;

    inline L_ALWAYS_INLINE void NoOpt(bool value)
    {
        DoNotOptimize(value);
    }

    inline L_ALWAYS_INLINE void DoCheck(bool value)
    {
        CHECK(value);
    }

    std::unordered_map<std::string, std::string>& GetTestDomains()
    {
        static std::unordered_map<std::string, std::string> domains;
        return domains;
    }

    int SetTestDomain(const std::string& func, cstring domain)
    {
        GetTestDomains()[func] = std::string(domain);
        return 0;
    }

    std::string SanitizeFunctionName(cstring func)
    {
        std::string_view fname = func;
        auto end = fname.find_first_of('(');
        fname = fname.substr(0, end);
        auto start = fname.find_last_of(' ');
        return std::string(fname.substr(start+1));
    }

    template<typename Callable, typename... Args>
    inline L_ALWAYS_INLINE void Benchmark_IMPL(const std::string& func, size_type n, Callable c, Args&&... args)
    {
        std::cout << '[' << GetTestDomains()[func] << "] Running benchmark\n\t";
        Check = &NoOpt;
        time::clock clck;
        clck.start();
        for (int i = 0; i < n; i++)
        {
            std::invoke(c, std::forward<Args>(args)...);
        }
        auto elapsed = clck.end();
        std::cout << n << " cases\n\t";
        std::cout << elapsed.milliseconds() / ((double)n) << "ms on average\n\t";
        std::cout << elapsed.milliseconds() << "ms total\n";
    }

    template<typename Callable, typename... Args>
    inline L_ALWAYS_INLINE void Test_IMPL(const std::string& func, Callable c, Args&&... args)
    {
        std::cout << '[' << GetTestDomains()[func] << "] Running test\n";
        Check = &DoCheck;
        std::invoke(c, std::forward<Args>(args)...);
    }

#define Benchmark_N(n, c, ...) legion::Benchmark_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), n, c, __VA_ARGS__)
#define Benchmark(c, ...) legion::Benchmark_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), 10000, c, __VA_ARGS__)

#define Test(c, ...) legion::Test_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), c, __VA_ARGS__)

#define LEGION_FUNCNAME_IMPL(f) #f

#define LEGION_REGISTER_TEST(f, domain)                                                                             \
            DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                                     \
            legion::SetTestDomain(LEGION_FUNCNAME_IMPL(f), domain);                                                 \
            DOCTEST_GLOBAL_NO_WARNINGS_END()                                                                        \
            static void f();                                                                                        \
            DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, f, domain)                                                     \
            static void f()

#define LEGION_TEST(domain) LEGION_REGISTER_TEST(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), domain)

}
