#pragma once
#include "unit_test.hpp"
#include <core/core.hpp>

using namespace legion;

struct test_comp
{
    int value;
};

void TestECS()
{
#pragma region basic component and entity behaviour
    {
        auto ent = ecs::Registry::createEntity();

        Check(ent);
        Check(ent != invalid_id);
        Check(ent != nullptr);
        Check(!(ent == nullptr));

        Check(ent == ent->id);
        Check(!ent.has_component<test_comp>());

        auto comp = ent.add_component<test_comp>();
        Check(comp);
        Check(ent.has_component<test_comp>());
        Check(comp->value == 0);
        comp->value++;
        Check(comp->value == 1);

        auto comp2 = ent.get_component<test_comp>();
        Check(comp2);
        Check(comp2 == comp);
        Check(comp2->value == 1);
        comp2->value++;
        Check(comp->value == 2);

        ent.remove_component<test_comp>();
        Check(!comp);
        Check(!comp2);
        Check(!ent.has_component<test_comp>());

        ent.add_component<test_comp>();
        Check(comp);
        Check(comp2);

        auto& val = comp.get();
        Check(val.value == 0);
        val.value++;
        Check(comp->value == 1);

        comp.destroy();
        Check(!comp);
        Check(!ent.has_component<test_comp>());

        ent.destroy();
        Check(!ent);
        Check(ent == invalid_id);
        Check(ent == nullptr);
        Check(!(ent != nullptr));

        auto ent2 = ecs::Registry::createEntity();
        Check(ent2 == ent);
        Check(!ent.has_component<test_comp>());
    }
#pragma endregion

#pragma region Hierarchy
    {
        auto parent = ecs::Registry::createEntity();
        Check(parent.get_parent() == ecs::world);
        Check(parent.children().size() == 0);
        for ([[maybe_unused]] auto& chld : parent)
            Check(false);

        auto child = ecs::Registry::createEntity(parent);
        Check(child.get_parent() == parent);
        Check(parent.children().size() == 1);
        size_type chldCount = 0;
        for ([[maybe_unused]] auto& chld : parent)
            chldCount++;
        Check(chldCount == 1);
    }
#pragma endregion
}

LEGION_TEST("core::ecs")
{
    Test(TestECS);

    Benchmark_N(100000, TestECS);
    Benchmark(TestECS);
}
