#pragma once

#include <core/core.hpp>
#include <memory>
#include <physics/diviner/halfedgeface.hpp>
#include <physics/diviner/data/convergence_identifier.hpp>
#include <physics/diviner/physics_contact.hpp>

namespace legion::physics
{
    struct physics_manifold;
    class ConvexCollider;


    class PhysicsCollider
    {
    public:
        bool shouldBeDrawn = true;
        std::vector<std::unique_ptr<ConvergenceIdentifier>> convergenceIdentifiers;

        PhysicsCollider()
        {
            static int colliderID = 0;
            id = colliderID++;
        }

        virtual void AddConvergenceIdentifier(const  physics_contact& contact) = 0;

        void AttemptFindAndCopyConvergenceID(physics_contact& contact)
        {
            if (!constants::applyWarmStarting) { return; }

            for (auto& convergenceId : convergenceIdentifiers)
            {
                if (convergenceId->refColliderID == contact.refCollider->GetColliderID())
                {
                    if (convergenceId->IsEqual(contact))
                    {
                        convergenceId->CopyLambdasToContact(contact);
                        return;
                    }
                }
            }
        }
            
        /** @brief given a PhysicsCollider, CheckCollision calls "CheckCollisionWith". Both colliders are then passed through
        * to the correct "CheckCollisionWith" function with double dispatch.
        * @param physicsCollider The collider we would like to check collision against
        * @param [in/out] manifold A physics_manifold that holds information about the collision
        */
        virtual void CheckCollision(
            PhysicsCollider* physicsCollider, physics_manifold& manifold) {};

        /** @brief given a convexCollider checks if this collider collides the convexCollider. The information
        * the information is then passed to the manifold.
        */
        virtual void CheckCollisionWith(ConvexCollider* convexCollider, physics_manifold& manifold) {};

        /** @brief Gets the unique id of this collider
        */
        int GetColliderID() const
        {
            return id;
        }

        /** @brief given a PhysicsCollider, PopulateContactPoints calls PopulateContactPointsWith. Both colliders are then passed through
        * to the corrent FillManifoldWith function with double dispatch.
        */
        virtual void PopulateContactPoints(
            PhysicsCollider* physicsCollider, physics_manifold& manifold) {};

        /** @brief Creates the contact points between this physics collider and the given ConvexCollider and
        * stores them in the manifold
        */
        virtual void PopulateContactPointsWith(
            ConvexCollider* convexCollider, physics_manifold& manifold) {};

        /** @brief Given the transform of the entity that the collider is attached to, draws a visual representation
        * of the collider.
        * @note This is called internally by PhysicsSysten
        */
        virtual void DrawColliderRepresentation(const math::mat4& transform, math::color usedColor, float width, float time, bool ignoreDepth = false) {};

        virtual void UpdateTransformedTightBoundingVolume(const math::mat4& transform) {};

        inline virtual std::vector<HalfEdgeFace*>& GetHalfEdgeFaces()
        {
            return dummyHalfEdges;
        }

        L_NODISCARD math::vec3 GetLocalCentroid() const noexcept
        {
            return localColliderCentroid;
        }

        //
        std::pair<math::vec3, math::vec3> GetMinMaxLocalAABB() const
        {
            return minMaxLocalAABB;
        }

        std::pair<math::vec3, math::vec3> GetMinMaxWorldAABB() const
        {
            return minMaxWorldAABB;
        }

    protected:

        math::vec3 localColliderCentroid = math::vec3(0, 0, 0);
        std::pair<math::vec3, math::vec3> minMaxLocalAABB;
        std::pair<math::vec3, math::vec3> minMaxWorldAABB;
    private:

        int id = -1;
       //this is not used, its mostly for debug reasons
        std::vector<HalfEdgeFace*> dummyHalfEdges;

    };
}



