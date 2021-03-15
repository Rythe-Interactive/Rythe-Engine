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

    std::string* lastDomain = nullptr;
    cstring lastSubDomain = nullptr;
    std::vector<cstring> subdomainnames = decltype(subdomainnames){};
    std::unordered_map<cstring, std::pair<size_type, long double>> subdomaintimes = decltype(subdomaintimes){};

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

    using CheckFunc = void(*)(bool);
    using SubDomainFunc = bool(*)(cstring);

    CheckFunc Check;
    SubDomainFunc StartSubDomain;

    struct SubDomainStack
    {
        static inline size_type items = 0;
        long double* time;
        time::clock<long double> clck;

        SubDomainStack(bool benchmark) : time(nullptr)
        {
            items++;
            if (benchmark)
            {
                time = &(subdomaintimes.at(lastSubDomain).second);
                clck.start();
            }
        }

        ~SubDomainStack()
        {
            if (time)
            {
                *time += clck.end().milliseconds();
            }
            items--;
        }

        constexpr operator bool() const noexcept { return true; }
    };

    inline L_ALWAYS_INLINE void NoOpt(bool value)
    {
        DoNotOptimize(value);
    }

    inline L_ALWAYS_INLINE void DoCheck(bool value)
    {
        CHECK(value);
    }

    inline L_ALWAYS_INLINE bool NoPrint(cstring val)
    {
        lastSubDomain = val;
        if (!subdomaintimes.count(val))
            subdomainnames.push_back(val);
        subdomaintimes.emplace(val, std::make_pair(SubDomainStack::items, (long double)0));
        return true;
    }

    inline L_ALWAYS_INLINE bool DoPrint(cstring val)
    {
        lastSubDomain = val;
        std::cout << '[' << (lastDomain ? *lastDomain : "unknown") << "] ";
        for (int i = 0; i <= SubDomainStack::items; i++)
            std::cout << "    ";
        std::cout << val << '\n';
        return false;
    }

    std::string SanitizeFunctionName(cstring func)
    {
        std::string_view fname = func;
        auto end = fname.find_first_of('(');
        fname = fname.substr(0, end);
        auto start = fname.find_last_of(' ');
        return std::string(fname.substr(start + 1));
    }

    template<typename Callable, typename... Args>
    inline L_ALWAYS_INLINE void Benchmark_IMPL(const std::string& func, size_type n, Callable c, Args&&... args)
    {
        subdomaintimes.clear();
        lastDomain = &GetTestDomains()[func];
        std::cout << '[' << *lastDomain << "] Running benchmark: " << n << " cases\n";
        Check = &NoOpt;
        StartSubDomain = &NoPrint;
        time::clock<long double> clck;
        clck.start();
        for (int i = 0; i < n; i++)
        {
            std::invoke(c, std::forward<Args>(args)...);
        }
        auto elapsed = clck.end();

        if (subdomainnames.size())
        {
            long double total = 0;
            for (auto& name : subdomainnames)
            {
                auto& [indent, time] = subdomaintimes.at(name);
                std::cout << '[' << *lastDomain << "] ";
                for (int i = 0; i <= indent; i++)
                    std::cout << "    ";
                std::cout << name << ": " << time << "ms total " << time / ((long double)n) << "ms on average\n";
                total += time;
            }
            subdomaintimes.clear();

            std::cout << '[' << *lastDomain << "]     " << total / ((long double)n) << "ms on average\n";
            std::cout << '[' << *lastDomain << "]     " << total << "ms total\n";
        }
        else
        {
            std::cout << '[' << *lastDomain << "]     " << elapsed.milliseconds() / ((long double)n) << "ms on average\n";
            std::cout << '[' << *lastDomain << "]     " << elapsed.milliseconds() << "ms total\n";
        }
    }

    template<typename Callable, typename... Args>
    inline L_ALWAYS_INLINE void Test_IMPL(const std::string& func, Callable c, Args&&... args)
    {
        lastDomain = &GetTestDomains()[func];
        std::cout << '[' << *lastDomain << "] Running test\n";
        Check = &DoCheck;
        StartSubDomain = &DoPrint;
        std::invoke(c, std::forward<Args>(args)...);
    }

#define Benchmark_N(n, ...) legion::Benchmark_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), n, __VA_ARGS__)
#define Benchmark(...) legion::Benchmark_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), 10000, __VA_ARGS__)

#define Test(...) legion::Test_IMPL(legion::SanitizeFunctionName(__FULL_FUNC__), __VA_ARGS__)

#define LEGION_FUNCNAME_IMPL(f) #f

#define LEGION_REGISTER_TEST(f, domain)                                                                             \
            DOCTEST_GLOBAL_NO_WARNINGS(DOCTEST_ANONYMOUS(_DOCTEST_ANON_VAR_)) =                                     \
            legion::SetTestDomain(LEGION_FUNCNAME_IMPL(f), domain);                                                 \
            DOCTEST_GLOBAL_NO_WARNINGS_END()                                                                        \
            static void f();                                                                                        \
            DOCTEST_REGISTER_FUNCTION(DOCTEST_EMPTY, f, domain)                                                     \
            static void f()

#define LEGION_TEST(domain) LEGION_REGISTER_TEST(DOCTEST_ANONYMOUS(_DOCTEST_ANON_FUNC_), domain)

#define LEGION_SUBTEST(subdomain) if(const SubDomainStack& DOCTEST_ANONYMOUS(_DOCTEST_ANON_SUBCASE_) DOCTEST_UNUSED = StartSubDomain(subdomain))

}
