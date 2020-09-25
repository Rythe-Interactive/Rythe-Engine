#pragma once
#include <physics/physicsimport.h>
#include <physics/convexcollider.hpp>

namespace args::physics
{
	struct HalfEdgeFace;

	class PhysicsStatics
	{
	public:

        /** @brief Given a transformed ConvexCollider and a direction, Gets the vertex furthest in the given direction
        * @param planePosition The position of the support plane in world space
        * @param direction The direction we would like to know the support point of
        * @param collider The ConvexCollider in question
        * @param colliderTransform A mat4 describing the transform of the collider
        * @param worldSupportPoint [out] the resulting support point
        */
        static void GetSupportPoint(math::vec3 planePosition, math::vec3 direction, ConvexCollider* collider, math::mat4 colliderTransform
            ,math::vec3& worldSupportPoint)
        {
            float largestDistanceInDirection = std::numeric_limits<float>::lowest();

            for (const auto& vert : collider->GetVertices())
            {
                math::vec3 transformedVert = colliderTransform * math::vec4(vert, 1);

                float dotResult = math::dot(transformedVert - planePosition, direction);

                if (dotResult > largestDistanceInDirection)
                {
                    largestDistanceInDirection = dotResult;
                    worldSupportPoint = transformedVert;
                }
            }
        }
		
        /** @param Given 2 ConvexColliders, convexA and convexB, checks if one of the faces of convexB creates a seperating axis
        * @param convexA the reference collider 
        * @param convexB the collider that will create the seperating axes
        * @param transformA the transform of convexA
        * @param transformB the transform of convexB
        * @param refFace [out] a HalfEdgeEdge* that has a normal parallel to the seperating axis
        * @param maximumSeperation [out] the seperation on the given seperating axis
        */
        static bool FindSeperatingAxisByExtremePointProjection(ConvexCollider* convexA
            , ConvexCollider* convexB, const math::mat4& transformA, const math::mat4& transformB, HalfEdgeFace* refFace, float& maximumSeperation)
        {
            float currentMaximumSeperation = std::numeric_limits<float>::lowest();
 

            for (const auto face : convexB->GetHalfEdgeFaces())
            {
                //get inverse normal
                math::vec3 seperatingAxis = transformB * math::vec4( face->normal, 0);

                math::vec3 transformedPositionB = transformB * math::vec4(face->centroid, 1);

                //get extreme point of other face in normal direction
                math::vec3 worldSupportPoint;
                GetSupportPoint(transformedPositionB, -seperatingAxis,
                    convexA, transformA, worldSupportPoint);

                float seperation = math::dot(worldSupportPoint - transformedPositionB, seperatingAxis);

                if (seperation > currentMaximumSeperation)
                {
                    currentMaximumSeperation = seperation;
                    refFace = face;
                }

                if (seperation > 0)
                {
                    //we have found a seperating axis, we can exit early
                    return true;
                }
            }
            //no seperating axis was found
            return false;
        }
	};
}
