#pragma once
#include "unit_test.hpp"
#include <core/core.hpp>

template<lgn::size_type I>
struct flag
{
    constexpr static lgn::size_type index = I;
};

template<typename _Flag>
struct test_struct : public _Flag
{
    static lgn::size_type ctor;
    static lgn::size_type mvtor;
    static lgn::size_type mvassign;
    static lgn::size_type cpytor;
    static lgn::size_type cpyassign;
    static lgn::size_type dtor;

    lgn::size_type val = 0;

    explicit test_struct(lgn::size_type i) noexcept : val(i) { ctor++; }
    test_struct() noexcept { ctor++; }
    test_struct(test_struct&& o) noexcept : val(o.val) { mvtor++; }
    test_struct(const test_struct& o) noexcept : val(o.val) { cpytor++; }
    test_struct& operator=(test_struct&& o) noexcept { val = o.val; mvassign++; return *this; }
    test_struct& operator=(const test_struct& o) noexcept { val = o.val; cpyassign++; return *this; }
    ~test_struct() noexcept { dtor++; }

    static void clear() noexcept
    {
        ctor = 0;
        mvtor = 0;
        mvassign = 0;
        cpytor = 0;
        cpyassign = 0;
        dtor = 0;
    }

    bool operator==(const test_struct& other) const noexcept { return val == other.val; }
    bool operator!=(const test_struct& other) const noexcept { return val != other.val; }
};

template<typename _Flag>
lgn::size_type test_struct<_Flag>::ctor = 0;
template<typename _Flag>
lgn::size_type test_struct<_Flag>::mvtor = 0;
template<typename _Flag>
lgn::size_type test_struct<_Flag>::mvassign = 0;
template<typename _Flag>
lgn::size_type test_struct<_Flag>::cpytor = 0;
template<typename _Flag>
lgn::size_type test_struct<_Flag>::cpyassign = 0;
template<typename _Flag>
lgn::size_type test_struct<_Flag>::dtor = 0;

namespace std
{
    template<typename _Flag>
    struct hash<test_struct<_Flag>>
    {
        size_t operator()(test_struct<_Flag> const& test) const noexcept
        {
            return test.val;
        }
    };
}

using key_flag = flag<0>;
using value_flag = flag<1>;
using key_struct = test_struct<key_flag>;
using value_struct = test_struct<value_flag>;

static void TestSparseMap()
{
    using namespace legion;

    LEGION_SUBTEST("Constructors")
    {
        {
            key_struct::clear();
            value_struct::clear();

            sparse_map<key_struct, value_struct> m;
            L_CHECK(m.size() == 0);
            L_CHECK(m.capacity() == static_cast<size_type>(1ull));
            L_CHECK(m.empty());
            L_CHECK(key_struct::ctor == 0);
            L_CHECK(key_struct::mvtor == 0);
            L_CHECK(key_struct::mvassign == 0);
            L_CHECK(key_struct::cpytor == 0);
            L_CHECK(key_struct::cpyassign == 0);
            L_CHECK(key_struct::dtor == 0);
            L_CHECK(value_struct::ctor == 0);
            L_CHECK(value_struct::mvtor == 0);
            L_CHECK(value_struct::mvassign == 0);
            L_CHECK(value_struct::cpytor == 0);
            L_CHECK(value_struct::cpyassign == 0);
            L_CHECK(value_struct::dtor == 0);
        }

        {
            key_struct::clear();
            value_struct::clear();

            sparse_map<key_struct, value_struct> m(1000);
            L_CHECK(m.size() == 0);
            L_CHECK(m.capacity() == static_cast<size_type>(1000ull));
            L_CHECK(m.empty());
            L_CHECK(key_struct::ctor == 0);
            L_CHECK(key_struct::mvtor == 0);
            L_CHECK(key_struct::mvassign == 0);
            L_CHECK(key_struct::cpytor == 0);
            L_CHECK(key_struct::cpyassign == 0);
            L_CHECK(key_struct::dtor == 0);
            L_CHECK(value_struct::ctor == 0);
            L_CHECK(value_struct::mvtor == 0);
            L_CHECK(value_struct::mvassign == 0);
            L_CHECK(value_struct::cpytor == 0);
            L_CHECK(value_struct::cpyassign == 0);
            L_CHECK(value_struct::dtor == 0);
        }
    }

    LEGION_SUBTEST("Insert & Emplace")
    {
        {
            key_struct::clear();
            value_struct::clear();

            sparse_map<key_struct, value_struct> m;
            m.reserve(1000);
            for (size_type i = 0; i < 1000; i++)
            {
                m.emplace(key_struct(i), value_struct(i));
            }

            //L_CHECK(m.size() == 1000);
            //L_CHECK(m.capacity() == static_cast<size_type>(1024ull));
            //L_CHECK(!m.empty());
            //L_CHECK(key_struct::ctor == 1000);
            //L_CHECK(key_struct::mvtor == 1000);
            //L_CHECK(key_struct::mvassign == 0);
            //L_CHECK(key_struct::cpytor == 1000);
            //L_CHECK(key_struct::cpyassign == 0);
            //L_CHECK(key_struct::dtor == 1000);
            //L_CHECK(value_struct::ctor == 1000);
            //L_CHECK(value_struct::mvtor == 1000);
            //L_CHECK(value_struct::mvassign == 0);
            //L_CHECK(value_struct::cpytor == 0);
            //L_CHECK(value_struct::cpyassign == 0);
            //L_CHECK(value_struct::dtor == 1000);
        }

        {
            key_struct::clear();
            value_struct::clear();

            sparse_map<key_struct, value_struct> m;
            m.reserve(1000);
            for (size_type i = 0; i < 1000; i++)
            {
                m.emplace(key_struct(i), i);
            }

            //L_CHECK(m.size() == 1000);
            //L_CHECK(m.capacity() == static_cast<size_type>(1024ull));
            //L_CHECK(!m.empty());
            //L_CHECK(key_struct::ctor == 1000);
            //L_CHECK(key_struct::mvtor == 1000);
            //L_CHECK(key_struct::mvassign == 0);
            //L_CHECK(key_struct::cpytor == 1000);
            //L_CHECK(key_struct::cpyassign == 0);
            //L_CHECK(key_struct::dtor == 1000);
            //L_CHECK(value_struct::ctor == 1000);
            //L_CHECK(value_struct::mvtor == 0);
            //L_CHECK(value_struct::mvassign == 0);
            //L_CHECK(value_struct::cpytor == 0);
            //L_CHECK(value_struct::cpyassign == 0);
            //L_CHECK(value_struct::dtor == 0);
        }
    }
}

LEGION_TEST("core::containers::sparse_containers")
{
    Test(TestSparseMap);

#if defined(LEGION_DEBUG)
    Benchmark(TestSparseMap);
#elif defined(LEGION_RELEASE)
    Benchmark_N(100000, TestSparseMap);
#else
    Benchmark_N(1, TestSparseMap);
#endif
}
