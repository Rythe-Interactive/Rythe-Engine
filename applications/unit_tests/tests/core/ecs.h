#pragma once
#include "doctest.h"
#include <core/core.hpp>

using namespace legion;

struct test_comp
{
    int value;
};

TEST_CASE("[core:ecs] testing the ECS")
{
    std::cout << "[core:ecs] testing the ECS\n";

#pragma region basic component and entity behaviour
    {
        auto ent = ecs::Registry::createEntity();

        CHECK(ent != invalid_id);
        CHECK(ent != nullptr);
        CHECK(!(ent == nullptr));

        CHECK(ent == ent.id);
        CHECK(!ent.has_component<test_comp>());

        auto comp = ent.add_component<test_comp>();
        CHECK(comp);
        CHECK(ent.has_component<test_comp>());
        CHECK(comp->value == 0);
        comp->value++;
        CHECK(comp->value == 1);

        auto comp2 = ent.get_component<test_comp>();
        CHECK(comp2);
        CHECK(comp2 == comp);
        CHECK(comp2->value == 1);
        comp2->value++;
        CHECK(comp->value == 2);

        ent.remove_component<test_comp>();
        CHECK(!comp);
        CHECK(!comp2);
        CHECK(!ent.has_component<test_comp>());

        ent.add_component<test_comp>();
        CHECK(comp);
        CHECK(comp2);

        auto& val = comp.get();
        CHECK(val.value == 0);
        val.value++;
        CHECK(comp->value == 1);

        comp.destroy();
        CHECK(!comp);
        CHECK(!ent.has_component<test_comp>());

        ent.destroy();
        CHECK(ent == nullptr);
        CHECK(!(ent != nullptr));

        auto ent2 = ecs::Registry::createEntity();
        CHECK(ent2 == ent);
        CHECK(!ent.has_component<test_comp>());
    }
#pragma endregion

#pragma region Hierarchy
    {
        auto parent = ecs::Registry::createEntity();
        CHECK(parent.get_parent() == ecs::world);
    }
#pragma endregion


}
