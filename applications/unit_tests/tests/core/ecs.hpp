#pragma once
#include "unit_test.hpp"
#include <core/core.hpp>


struct test_comp
{
    int value;
};

static void TestECS()
{
    using namespace legion;
    ecs::Registry::clear();

    LEGION_SUBTEST("Basic component and entity behaviour")
    {
        auto ent = ecs::Registry::createEntity();
        id_type entId = ent;

        L_CHECK(ent);
        L_CHECK(ent != invalid_id);
        L_CHECK(ent != nullptr);
        L_CHECK(!(ent == nullptr));

        L_CHECK(ent == ent->id);
        L_CHECK(!ent.has_component<test_comp>());

        auto comp = ent.add_component<test_comp>();
        L_CHECK(comp);
        L_CHECK(ent.has_component<test_comp>());
        L_CHECK(comp->value == 0);
        comp->value++;
        L_CHECK(comp->value == 1);
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto comp2 = ent.get_component<test_comp>();
        L_CHECK(comp2);
        L_CHECK(comp2 == comp);
        L_CHECK(comp2->value == 1);
        comp2->value++;
        L_CHECK(comp->value == 2);

        ent.remove_component<test_comp>();
        L_CHECK(!comp);
        L_CHECK(!comp2);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        L_CHECK(comp);
        L_CHECK(comp2);

        auto& val = comp.get();
        L_CHECK(val.value == 0);
        val.value++;
        L_CHECK(comp->value == 1);

        comp.destroy();
        L_CHECK(!comp);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        ent.destroy();
        L_CHECK(!ent);
        L_CHECK(ent == invalid_id);
        L_CHECK(ent == nullptr);
        L_CHECK(!(ent != nullptr));
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto ent2 = ecs::Registry::createEntity();
        L_CHECK(ent2 == ent);
        L_CHECK(!ent.has_component<test_comp>());
        ent2.destroy();
    }

    LEGION_SUBTEST("Multi-op and archetypes")
    {
        auto ent = ecs::Registry::createEntity();
        id_type entId = ent;

        L_CHECK(ent);
        L_CHECK(ent != invalid_id);
        L_CHECK(ent != nullptr);
        L_CHECK(!(ent == nullptr));

        L_CHECK(ent == ent->id);
        L_CHECK(!ent.has_component<test_comp>());

        auto comp = ent.add_component<test_comp>();
        L_CHECK(comp);
        L_CHECK(ent.has_component<test_comp>());
        L_CHECK(comp->value == 0);
        comp->value++;
        L_CHECK(comp->value == 1);
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto comp2 = ent.get_component<test_comp>();
        L_CHECK(comp2);
        L_CHECK(comp2 == comp);
        L_CHECK(comp2->value == 1);
        comp2->value++;
        L_CHECK(comp->value == 2);

        ent.remove_component<test_comp>();
        L_CHECK(!comp);
        L_CHECK(!comp2);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        L_CHECK(comp);
        L_CHECK(comp2);

        auto& val = comp.get();
        L_CHECK(val.value == 0);
        val.value++;
        L_CHECK(comp->value == 1);

        comp.destroy();
        L_CHECK(!comp);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        ent.destroy();
        L_CHECK(!ent);
        L_CHECK(ent == invalid_id);
        L_CHECK(ent == nullptr);
        L_CHECK(!(ent != nullptr));
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(entId));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto ent2 = ecs::Registry::createEntity();
        L_CHECK(ent2 == ent);
        L_CHECK(!ent.has_component<test_comp>());
        ent2.destroy();
    }

    LEGION_SUBTEST("Hierarchy")
    {
        auto parent = ecs::Registry::createEntity();
        L_CHECK(parent.get_parent() == ecs::world);
        L_CHECK(parent.children().size() == 0);
        for (L_MAYBEUNUSED auto& chld : parent)
            L_CHECK(false);

        auto child = ecs::Registry::createEntity(parent);
        L_CHECK(child.get_parent() == parent);
        L_CHECK(parent.children().size() == 1);
        size_type chldCount = 0;
        for (auto& chld : parent)
        {
            L_CHECK(chld == child);
            chldCount++;
        }
        L_CHECK(chldCount == 1);

        parent.remove_child(child);
        L_CHECK(parent.children().size() == 0);
        L_CHECK(child.get_parent() == ecs::world);

        child.set_parent(parent);
        L_CHECK(child.get_parent() == parent);
        L_CHECK(parent.children().size() == 1);

        parent.remove_children();
        L_CHECK(parent.children().size() == 0);
        L_CHECK(child.get_parent() == ecs::world);

        parent.add_child(child);
        L_CHECK(child.get_parent() == parent);
        L_CHECK(parent.children().size() == 1);

        parent.destroy_children();
        L_CHECK(parent.children().size() == 0);
        L_CHECK(!child);
        L_CHECK(child == invalid_id);
        L_CHECK(child == nullptr);
        L_CHECK(!(child != nullptr));

        child = ecs::Registry::createEntity(parent);
        L_CHECK(child);

        parent.destroy();
        L_CHECK(!parent);
        L_CHECK(parent == invalid_id);
        L_CHECK(parent == nullptr);
        L_CHECK(!(parent != nullptr));
        L_CHECK(!child);
        L_CHECK(child == invalid_id);
        L_CHECK(child == nullptr);
        L_CHECK(!(child != nullptr));
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
        L_CHECK(ecs::world->children.size() == 0);
    }

    LEGION_SUBTEST("Filters")
    {
        ecs::filter<test_comp> fltr;
        for (L_MAYBEUNUSED auto& ent : fltr)
            L_CHECK(false);


        L_CHECK(ecs::component_pool<test_comp>::m_components.empty());

        for (int i = 0; i < 100; i++)
        {
            L_CHECK(ecs::component_pool<test_comp>::m_components.size() == i);

            auto ent = ecs::Registry::createEntity();
            ent.add_component<test_comp>();
        }

        int count = 0;

        for (auto& ent : fltr)
        {
            if (ent.has_component<test_comp>())
            {
                ent.get_component<test_comp>()->value = 1;
                count++;
            }
            else
                L_CHECK(false);
        }

        L_CHECK(count == 100 && count == static_cast<int>(fltr.size()));
        count = 0;
        for (auto& ent : fltr.reverse_range())
        {
            count += ent.get_component<test_comp>()->value;
            ent.destroy();
        }

        L_CHECK(count == 100);
        L_CHECK(fltr.size() == 0);
        L_CHECK(ecs::component_pool<test_comp>::m_components.empty());
    }
}

LEGION_TEST("core::ecs")
{
    Test(TestECS);

#if defined(LEGION_DEBUG)
    Benchmark(TestECS);
#elif defined(LEGION_RELEASE)
    Benchmark_N(100000, TestECS);
#else
    Benchmark_N(1, TestECS);
#endif
}
