#pragma once
#include "unit_test.hpp"
#include <core/core.hpp>

static void TestFunc(int& ref) { ref++; }

struct TestFunctor
{
    int& ref;
    void operator()() { ref++; }
};

static void TestDelegate()
{
    using namespace legion;

    LEGION_SUBTEST("Delegate")
    {
        delegate<void()> del;
        L_CHECK(!del);
        L_CHECK(del == nullptr);
        L_CHECK(!(del != nullptr));

        int value = 0;

        del();
        L_CHECK(value == 0);

        del = [&]() { value++; };
        L_CHECK(static_cast<bool>(del));
        L_CHECK(del != nullptr);
        L_CHECK(!(del == nullptr));

        del();
        L_CHECK(value == 1);

        del.reset();
        L_CHECK(!del);
        L_CHECK(del == nullptr);
        L_CHECK(!(del != nullptr));

        del = TestFunctor{ value };
        L_CHECK(static_cast<bool>(del));
        L_CHECK(del != nullptr);
        L_CHECK(!(del == nullptr));

        del();
        L_CHECK(value == 2);

        del = nullptr;
        L_CHECK(!del);
        L_CHECK(del == nullptr);
        L_CHECK(!(del != nullptr));

        delegate<void(int&)> delParam;

        delParam = TestFunc;
        L_CHECK(static_cast<bool>(delParam));
        L_CHECK(delParam != nullptr);
        L_CHECK(!(delParam == nullptr));

        delParam(value);
        L_CHECK(value == 3);
    }

    LEGION_SUBTEST("Multicast delegate")
    {
        multicast_delegate<void()> del;
        L_CHECK(!del);
        L_CHECK(del.empty());

        int value = 0;

        delegate<void()> singleDel = [&]() { value++; };

        del();
        L_CHECK(value == 0);

        del.insert_back(singleDel);
        L_CHECK(static_cast<bool>(del));
        L_CHECK(!del.empty());
        L_CHECK(del.size() == 1);

        del();
        L_CHECK(value == 1);

        del.emplace_back([&]() { value++; });
        L_CHECK(static_cast<bool>(del));
        L_CHECK(del.size() == 2);

        del();
        L_CHECK(value == 3);

        TestFunctor functor{ value };
        del.emplace_back(functor);
        L_CHECK(del.size() == 3);

        del();
        L_CHECK(value == 6);

        del.erase(singleDel);
        L_CHECK(del.size() == 2);

        del();
        L_CHECK(value == 8);

        value = 0;
        multicast_delegate<void(int&)>  delParam;
        L_CHECK(!delParam);
        L_CHECK(delParam.empty());

        delParam(value);
        L_CHECK(value == 0);

        delParam.emplace_back(TestFunc);
        L_CHECK(static_cast<bool>(delParam));
        L_CHECK(!delParam.empty());
        L_CHECK(delParam.size() == 1);

        delParam(value);
        L_CHECK(value == 1);

        delegate<void(int&)> singleDelParam = TestFunc;

        delParam.insert_back(singleDelParam);
        L_CHECK(static_cast<bool>(delParam));
        L_CHECK(!delParam.empty());
        L_CHECK(delParam.size() == 2);

        delParam(value);
        L_CHECK(value == 3);

        delParam.erase(singleDelParam);
        L_CHECK(static_cast<bool>(delParam));
        L_CHECK(!delParam.empty());
        L_CHECK(delParam.size() == 1);

        delParam(value);
        L_CHECK(value == 4);
    }
}

LEGION_TEST("core::containers::delegate")
{
    Test(TestDelegate);

#if defined(LEGION_DEBUG)
    Benchmark(TestDelegate);
#elif defined(LEGION_RELEASE)
    Benchmark_N(100000, TestDelegate);
#else
    Benchmark_N(1, TestDelegate);
#endif
}
