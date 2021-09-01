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
    ecs::component_pool<test_comp>::reserve(1000);
    ecs::component_pool<position>::reserve(1000);
    ecs::component_pool<rotation>::reserve(1000);
    ecs::component_pool<scale>::reserve(1000);

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
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(ent));
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
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto startVal = test_comp{ 13 };
        ent.add_component(startVal);
        L_CHECK(comp);
        L_CHECK(comp2);

        auto& val = comp.get();
        L_CHECK(val.value == startVal.value);
        val.value++;
        L_CHECK(comp->value == startVal.value + 1);

        comp.destroy();
        L_CHECK(!comp);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        ent.add_component<test_comp>();
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        ent.destroy();
        L_CHECK(!ent);
        L_CHECK(ent == invalid_id);
        L_CHECK(ent == nullptr);
        L_CHECK(!(ent != nullptr));
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        auto ent2 = ecs::Registry::createEntity();
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent2));
        L_CHECK(!ecs::Registry::entityComposition(ent2->id).count(make_hash<test_comp>()));
        L_CHECK(!ent2.has_component<test_comp>());
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
        bool tempBool = !ent.has_component<test_comp, transform>();
        L_CHECK(tempBool);
        tempBool = !ent.has_component<test_comp, position, rotation, scale>();
        L_CHECK(tempBool);

        auto [comp, transf] = ent.add_component<test_comp, transform>();
        auto [pos, rot, scal] = transf;
        DoNotOptimize(pos);
        DoNotOptimize(rot);
        DoNotOptimize(scal);

        L_CHECK(comp);
        tempBool = ent.has_component<test_comp, transform>();
        L_CHECK(tempBool);
        tempBool = ent.has_component<position, test_comp, scale, rotation>();
        L_CHECK(tempBool);
        L_CHECK(comp->value == 0);
        comp->value++;
        L_CHECK(comp->value == 1);
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<scale>()));

        auto [comp2, transf2] = ent.get_component<test_comp, transform>();
        L_CHECK(comp2);
        L_CHECK(comp2 == comp);
        L_CHECK(comp2->value == 1);
        comp2->value++;
        L_CHECK(comp->value == 2);

        ent.remove_component<test_comp>();
        L_CHECK(!comp);
        L_CHECK(!comp2);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        L_CHECK(ent.has_component<transform>());
        tempBool = ent.has_component<position, rotation, scale>();
        L_CHECK(tempBool);

        ent.remove_component<transform>();
        L_CHECK(!comp);
        L_CHECK(!comp2);
        L_CHECK(!ent.has_component<transform>());
        L_CHECK(!ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<scale>()));


        ent.add_component<scale, rotation, position>();
        L_CHECK(ent.has_component<transform>());
        tempBool = ent.has_component<rotation, scale, position>();
        L_CHECK(tempBool);
        L_CHECK(ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<scale>()));

        ent.remove_component<position, scale, rotation>();
        L_CHECK(!ent.has_component<transform>());
        tempBool = !ent.has_component<rotation, position, scale>();
        L_CHECK(tempBool);
        L_CHECK(!ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<scale>()));


        auto startVal = test_comp{ 13 };
        position posVal;
        rotation rotVal;
        scale scalVal;
        ent.add_component(startVal, posVal, scalVal, rotVal);
        L_CHECK(comp);
        L_CHECK(comp2);
        L_CHECK(transf);
        L_CHECK(transf2);

        auto& val = comp.get();
        L_CHECK(val.value == startVal.value);
        val.value++;
        L_CHECK(comp->value == startVal.value + 1);

        comp.destroy();
        L_CHECK(!comp);
        L_CHECK(!ent.has_component<test_comp>());
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));

        transf.destroy();
        L_CHECK(!transf);
        L_CHECK(!ent.has_component<transform>());
        L_CHECK(!ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<scale>()));

        ent.add_component<test_comp>();
        L_CHECK(ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        ent.destroy();
        L_CHECK(!ent);
        L_CHECK(ent == invalid_id);
        L_CHECK(ent == nullptr);
        L_CHECK(!(ent != nullptr));
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<test_comp>()));
        L_CHECK(!ecs::component_pool<position>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<rotation>::contains_direct(ent));
        L_CHECK(!ecs::component_pool<scale>::contains_direct(ent));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<position>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<rotation>()));
        L_CHECK(!ecs::Registry::entityComposition(entId).count(make_hash<scale>()));

        auto ent2 = ecs::Registry::createEntity();
        L_CHECK(!ecs::component_pool<test_comp>::contains_direct(ent2));
        L_CHECK(!ecs::Registry::entityComposition(ent2->id).count(make_hash<test_comp>()));
        L_CHECK(!ecs::component_pool<position>::contains_direct(ent2));
        L_CHECK(!ecs::component_pool<rotation>::contains_direct(ent2));
        L_CHECK(!ecs::component_pool<scale>::contains_direct(ent2));
        L_CHECK(!ecs::Registry::entityComposition(ent2->id).count(make_hash<position>()));
        L_CHECK(!ecs::Registry::entityComposition(ent2->id).count(make_hash<rotation>()));
        L_CHECK(!ecs::Registry::entityComposition(ent2->id).count(make_hash<scale>()));
        L_CHECK(!ent2.has_component<test_comp>());
        L_CHECK(!ent2.has_component<transform>());
        tempBool = !ent2.has_component<position, rotation, scale>();
        L_CHECK(tempBool);
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

    std::array<ecs::entity, 1000> entities;
    for (size_type i = 0; i < entities.size(); i++)
    {
        entities[i] = ecs::Registry::createEntity();
    }

    LEGION_SUBTEST("Create 1000 components")
    {
        for (auto& ent : entities)
            ent.add_component<position>();
    }

    LEGION_SUBTEST("Destroy 1000 components")
    {
        for (auto& ent : entities)
            ent.remove_component<position>();
    }

    ecs::world.destroy_children();

    LEGION_SUBTEST("Create 1000 entities")
    {
        for (int i = 0; i < 1000; i++)
            (void)ecs::Registry::createEntity();
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

        for (size_type i = 0; i < 100; i++)
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

        if (!test_info::isBenchMarking)
            log::info("filter size: {}", fltr.size());

        L_CHECK(fltr.size() == 0);
        L_CHECK(ecs::component_pool<test_comp>::m_components.empty());
    }
}

LEGION_TEST("core::ecs")
{
    Test(TestECS);

#if defined(LEGION_DEBUG)
    Benchmark_N(100, TestECS);
#elif defined(LEGION_RELEASE)
    Benchmark_N(100000, TestECS);
#else
    Benchmark_N(1, TestECS);
#endif
}
