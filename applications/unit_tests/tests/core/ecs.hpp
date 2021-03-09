#pragma once
#include "unit_test.hpp"
#include <core/core.hpp>

using namespace legion;

struct test_comp
{
    int value;
};

template<bool leak>
void TestECS()
{
    LEGION_SUBTEST("Basic component and entity behaviour")
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
        if constexpr (leak)
        {
            ent2.destroy();
        }
    }

    LEGION_SUBTEST("Hierarchy")
    {
        auto parent = ecs::Registry::createEntity();
        Check(parent.get_parent() == ecs::world);
        Check(parent.children().size() == 0);
        for (L_MAYBEUNUSED auto& chld : parent)
            Check(false);

        auto child = ecs::Registry::createEntity(parent);
        Check(child.get_parent() == parent);
        Check(parent.children().size() == 1);
        size_type chldCount = 0;
        for (auto& chld : parent)
        {
            Check(chld == child);
            chldCount++;
        }
        Check(chldCount == 1);

        parent.remove_child(child);
        Check(parent.children().size() == 0);
        Check(child.get_parent() == ecs::world);

        child.set_parent(parent);
        Check(child.get_parent() == parent);
        Check(parent.children().size() == 1);

        parent.remove_children();
        Check(parent.children().size() == 0);
        Check(child.get_parent() == ecs::world);

        parent.add_child(child);
        Check(child.get_parent() == parent);
        Check(parent.children().size() == 1);

        parent.destroy_children();
        Check(parent.children().size() == 0);
        Check(!child);
        Check(child == invalid_id);
        Check(child == nullptr);
        Check(!(child != nullptr));

        child = ecs::Registry::createEntity(parent);
        Check(child);

        parent.destroy();
        Check(!parent);
        Check(parent == invalid_id);
        Check(parent == nullptr);
        Check(!(parent != nullptr));
        Check(!child);
        Check(child == invalid_id);
        Check(child == nullptr);
        Check(!(child != nullptr));
    }

    ecs::world.destroy_children();

    LEGION_SUBTEST("Filters")
    {
        ecs::filter<test_comp> fltr;
        for (auto& ent : fltr)
            Check(false);
    }
}

LEGION_TEST("core::ecs")
{
    Test(TestECS<false>);

    Benchmark_N(1000000, TestECS<true>);
}
