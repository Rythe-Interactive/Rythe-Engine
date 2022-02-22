#pragma once
#include "unit_test.hpp"
#include <physics/generic_components/generic_rigidbody.h>

static void TestRigidbody()
{
    using namespace legion;

    physics::genericRigidbody genericRigidbody;

    LEGION_SUBTEST("Rigidbody Read and Write")
    {
        physics::rigidbodyData& rbData = genericRigidbody.rigidbodyData;

        rbData.setMass(3.0f);
        L_CHECK(rbData.getMass() == 3.0f);
           
        rbData.setInertiaTensor(math::mat3(5.0f));
        L_CHECK(rbData.getInertiaTensor() == math::mat3(5.0f));

        rbData.setVelocity(math::vec3(5.0f));
        L_CHECK(rbData.getVelocity() == math::vec3(5.0f));

        rbData.setLinearDrag(0.02f);
        L_CHECK(rbData.getLinearDrag() == 0.02f);

        rbData.setAngularDrag(0.05f);
        L_CHECK(rbData.getAngularDrag() == 0.05f);

        LEGION_SUBTEST("Rigidbody Event Generation")
        {
            auto& modifyEvents = rbData.getModifyEvents();
            L_CHECK(modifyEvents.size() == 5);

            L_CHECK(modifyEvents[0].get()->get_id() == physics::rb_modify_mass::id);

            L_CHECK(modifyEvents[1].get()->get_id() == physics::rb_modify_inertia_tensor::id);

            L_CHECK(modifyEvents[2].get()->get_id() == physics::rb_modify_velocity::id);

            L_CHECK(modifyEvents[3].get()->get_id() == physics::rb_modify_linear_drag::id);

            L_CHECK(modifyEvents[4].get()->get_id() == physics::rb_modify_angular_drag::id);
        }
    }
}

LEGION_TEST("physics::rigidbody")
{
    Test(TestRigidbody);
}
