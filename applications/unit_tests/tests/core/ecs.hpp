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
    LEGION_SUBTEST("Basic component and entity behaviour")
    {
        auto ent = ecs::Registry::createEntity();
        id_type entId = ent;

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
        Check(ecs::component_pool<test_comp>::contains_direct(entId));
        Check(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

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
        Check(!ecs::component_pool<test_comp>::contains_direct(entId));
        Check(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

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
        Check(!ecs::component_pool<test_comp>::contains_direct(entId));
        Check(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        Check(ecs::component_pool<test_comp>::contains_direct(entId));
        Check(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        ent.destroy();
        Check(!ent);
        Check(ent == invalid_id);
        Check(ent == nullptr);
        Check(!(ent != nullptr));
        Check(!ecs::component_pool<test_comp>::contains_direct(entId));
        Check(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto ent2 = ecs::Registry::createEntity();
        Check(ent2 == ent);
        Check(!ent.has_component<test_comp>());
        ent2.destroy();
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

    LEGION_SUBTEST("Create 1000 entities")
    {
        for (int i = 0; i < 1000; i++)
            ecs::Registry::createEntity();
    }

    LEGION_SUBTEST("Destroy all entities")
    {
        ecs::world.destroy_children();
        Check(ecs::world->children.size() == 0);
    }

    LEGION_SUBTEST("Filters")
    {
        ecs::filter<test_comp> fltr;
        for (L_MAYBEUNUSED auto& ent : fltr)
            Check(false);


        for (int i = 0; i < 100; i++)
        {
            auto ent = ecs::Registry::createEntity();
            ent.add_component<test_comp>();
        }

        size_type count = 0;

        for (auto& ent : fltr)
        {
            if (ent.has_component<test_comp>())
            {
                ent.get_component<test_comp>()->value = 1;
                count++;
            }
            else
                Check(false);
        }

        Check(count == 100 && count == fltr.size());
        count = 0;
        for (auto& ent : fltr.reverse_range())
        {
            count += ent.get_component<test_comp>()->value;
            ent.destroy();
        }

        Check(count == 100);
        Check(fltr.size() == 0);
    }
}

LEGION_TEST("core::ecs")
{
    Test(TestECS);

#if defined(LEGION_DEBUG)
    Benchmark_N(10000, TestECS);
#elif defined(LEGION_RELEASE)
    Benchmark_N(100000, TestECS);
#else
    Benchmark_N(1, TestECS);
#endif
}