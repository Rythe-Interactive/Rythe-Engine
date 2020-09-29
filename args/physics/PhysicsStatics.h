#pragma once
#include <physics/physicsimport.h>
#include <physics/colliders/convexcollider.hpp>

namespace args::physics
{
	struct HalfEdgeFace;

	class PhysicsStatics
	{
	public:

        //---------------------------------------------------------------- Collision Detection ----------------------------------------------------------------------------//

        /** @brief Given a transformed ConvexCollider and a direction, Gets the vertex furthest in the given direction
         * @param planePosition The position of the support plane in world space
         * @param direction The direction we would like to know the support point of
         * @param collider The ConvexCollider in question
         * @param colliderTransform A mat4 describing the transform of the collider
         * @param worldSupportPoint [out] the resulting support point
         * @return returns true if a seperating axis was found
         */
        static void GetSupportPoint(const math::vec3& planePosition, const math::vec3& direction, ConvexCollider* collider, const math::mat4& colliderTransform
            , math::vec3& worldSupportPoint)
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
		
        /** @brief Given 2 ConvexColliders, convexA and convexB, checks if one of the faces of convexB creates a seperating axis 
         * that seperates the given convex shapes
         * @param convexA the reference collider 
         * @param convexB the collider that will create the seperating axes
         * @param transformA the transform of convexA
         * @param transformB the transform of convexB
         * @param refFace [out] a HalfEdgeEdge* that has a normal parallel to the seperating axis
         * @param maximumSeperation [out] the seperation on the given seperating axis
         * @return returns true if a seperating axis was found
         */
        static bool FindSeperatingAxisByExtremePointProjection(ConvexCollider* convexA
            , ConvexCollider* convexB, const math::mat4& transformA, const math::mat4& transformB, HalfEdgeFace** refFace, float& maximumSeperation) 
        {
            float currentMaximumSeperation = std::numeric_limits<float>::lowest();


            for ( auto face : convexB->GetHalfEdgeFaces())
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
                    *refFace = face;
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

        /** @brief Given 2 ConvexColliders, Goes through every single possible edge combination in order to check for a seperating axis. This is done
         * by creating a minkowski face with each edge combination.
         * @param convexA the reference collider 
         * @param convexB the incident collider
         * @param transformA the transform of convexA
         * @param transformB the transform of convexB
         * @param refEdge [out] the resulting reference Edge
         * @param incEdge [out] the resulting incident Edge
         * @param seperatingAxisFound [out] the resulting seperating axis found
         * @param seperation [out] the amount of seperation
         * @return returns true if a seperating axis was found
         */
        static bool FindSeperatingAxisByGaussMapEdgeCheck(ConvexCollider* convexA, ConvexCollider* convexB, 
            const math::mat4& transformA, const math::mat4& transformB,HalfEdgeEdge** refEdge,HalfEdgeEdge** incEdge,
            math::vec3& seperatingAxisFound,float & seperation)
        {
            float currentMaximumSeperation = std::numeric_limits<float>::lowest();
      

            math::vec3 positionA = transformA[3];

            seperation = 0.0f;

            for (auto faceA : convexA->GetHalfEdgeFaces())
            {
                //----------------- Get all edges of faceA ------------//

                std::vector<HalfEdgeEdge*> convexAHalfEdges;

                auto lambda = [&convexAHalfEdges](HalfEdgeEdge* edge)
                {
                    convexAHalfEdges.push_back(edge);
                };

                faceA->forEachEdge(lambda);

                for (auto faceB : convexB->GetHalfEdgeFaces())
                {
                    //----------------- Get all edges of faceB ------------//

                    std::vector<HalfEdgeEdge*> convexBHalfEdges;

                    auto lambda = [&convexBHalfEdges](HalfEdgeEdge* edge)
                    {
                        convexBHalfEdges.push_back(edge);
                    };

                    faceB->forEachEdge(lambda);

                    for (HalfEdgeEdge* edgeA : convexAHalfEdges)
                    {
                        for (HalfEdgeEdge* edgeB : convexBHalfEdges)
                        {
                            //if the given edges creates a minkowski face
                            if (attemptBuildMinkowskiFace(edgeA,edgeB,transformA,transformB))
                            {
                                //get world edge direction
                                math::vec3 edgeANormal = transformA * math::vec4(edgeA->getLocalEdgeDirection(), 0);
                                math::vec3 edgeBNormal = transformB * math::vec4(edgeB->getLocalEdgeDirection(), 0);

                                edgeANormal = math::normalize(edgeANormal);
                                edgeBNormal = math::normalize(edgeBNormal);

                                //get the seperating axis
                                math::vec3 seperatingAxis = math::normalize(math::cross(edgeANormal, edgeBNormal));

                                if (math::epsilonEqual(math::length(seperatingAxis), 0.0f, math::epsilon<float>()))
                                {
                                    continue;
                                }

                                //get world edge position
                                math::vec3 edgeAtransformedPosition = transformA * math::vec4(*edgeA->edgePositionPtr, 1);
                                math::vec3 edgeBtransformedPosition = transformB * math::vec4(*edgeB->edgePositionPtr, 1);

                                //check if its pointing in the right direction 
                                if (math::dot(seperatingAxis, edgeAtransformedPosition - positionA) < 0)
                                {
                                    seperatingAxis = -seperatingAxis;
                                }

                                //check if given edges create a seperating axis
                                float distance = math::dot(seperatingAxis, edgeBtransformedPosition - edgeAtransformedPosition);

                                if (currentMaximumSeperation > distance)
                                {
                                    refEdge = &edgeA;
                                    incEdge = &edgeB;
                                    seperatingAxisFound = seperatingAxis;
                                    currentMaximumSeperation = distance;
                                    seperation = distance;
                                }

                                if (distance > 0.0f)
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }


            return false;
        }

        private:

        //--------------------------------------------- private helper functions -------------------------------------------------------//

        /** @brief Given 2 HalfEdgeEdges and their respective transforms, transforms their normals and checks if they create a minkowski face
         * @return returns true if a minkowski face was succesfully constructed
         */
        static bool attemptBuildMinkowskiFace(HalfEdgeEdge* edgeA,HalfEdgeEdge* edgeB,const math::mat4& transformA, 
            const math::mat4& transformB)
        {
            const math::vec3 transformedA1 = transformA * math::vec4(edgeA->getLocalNormal(),0);
            const math::vec3 transformedA2 = transformA * math::vec4(edgeA->pairingEdge->getLocalNormal(), 0);

            const math::vec3 transformedB1 = transformB * math::vec4(edgeB->getLocalNormal(), 0);
            const math::vec3 transformedB2 = transformB * math::vec4(edgeB->pairingEdge->getLocalNormal(), 0);

            return isMinkowskiFace(transformedA1,transformedA2,-transformedB1,-transformedB2);
        }

        /** @brief Given 2 arcs, one that starts from transformedA1 and ends at transformedA2 and another arc 
         * that starts at transformedB1 and ends at transformedB2, checks if the given arcs collider each other
         * @return returns true if the given arcs intersect
         */
        static bool isMinkowskiFace(const math::vec3 transformedA1,const math::vec3 transformedA2,
            const math::vec3 transformedB1,const math::vec3 transformedB2)
        {
            //------------------------ Check if normals created by arcA seperate normals of B --------------------------------------//

            math::vec3 planeANormal = math::cross(transformedA1, transformedA2);

            float planeADotB1 = math::dot(planeANormal, transformedB1);
            float planeADotB2 = math::dot(planeANormal, transformedB2);

            float dotMultiplyResultA = 
                planeADotB1 * planeADotB2;

            if (dotMultiplyResultA > 0.0f ||  math::epsilonEqual(dotMultiplyResultA,0.0f,math::epsilon<float>()))
            {
                return false;
            }

            //------------------------ Check if normals created by arcB seperate normals of A --------------------------------------//

            math::vec3 planeBNormal = math::cross(transformedB1, transformedB2);

            float planeBDotA1 = math::dot(planeBNormal, transformedA1);
            float planeBDotA2 = math::dot(planeBNormal, transformedA2);

            float  dotMultiplyResultB = planeBDotA1 * planeBDotA2;

            if (dotMultiplyResultB > 0.0f || math::epsilonEqual(dotMultiplyResultB, 0.0f, math::epsilon<float>()))
            {
                return false;
            }

            //------------------------ Check if arcA and arcB are in the same hemisphere --------------------------------------------//

            math::vec3 abNormal = math::cross(transformedA2, transformedB2);

            float planeABDotA1 = math::dot(abNormal, transformedA1);
            float planeABDotB1 = math::dot(abNormal, transformedB1);

            float dotMultiplyResultAB = planeABDotA1 * planeABDotB1;

            if (dotMultiplyResultAB < 0.0f || math::epsilonEqual(dotMultiplyResultAB, 0.0f, math::epsilon<float>()))
            {
                return false;
            }

            return true;
        }
	};
}
