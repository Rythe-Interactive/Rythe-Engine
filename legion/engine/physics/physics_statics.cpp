#include <core/core.hpp>
#include <physics/physics_statics.hpp>
#include <rendering/debugrendering.hpp>
namespace legion::physics
{
    void PhysicsStatics::DetectConvexConvexCollision(ConvexCollider* convexA, ConvexCollider* convexB, const math::mat4& transformA, const math::mat4& transformB
        , ConvexConvexCollisionInfo& outCollisionInfo,physics_manifold& manifold)
    {
        //'this' is colliderB and 'convexCollider' is colliderA

        outCollisionInfo.ARefSeperation = 0.0f;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(
            convexB, convexA, transformB, transformA, outCollisionInfo.ARefFace, outCollisionInfo.ARefSeperation) || !outCollisionInfo.ARefFace.ptr)
        {
            //log::debug("Not Found on A ");
            return;
        }


        //log::debug("Face Check B");
        outCollisionInfo.BRefSeperation = 0.0f;
        if (PhysicsStatics::FindSeperatingAxisByExtremePointProjection(convexA,
            convexB, transformA,transformB, outCollisionInfo.BRefFace, outCollisionInfo.BRefSeperation) || !outCollisionInfo.BRefFace.ptr)
        {
            //log::debug("Not Found on B ");
            return;
        }

        PointerEncapsulator< HalfEdgeEdge> edgeRef;
        PointerEncapsulator< HalfEdgeEdge> edgeInc;


        outCollisionInfo.aToBEdgeSeperation =0.0f;
        //log::debug("Edge Check");
        if (PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck(convexB, convexA, transformB,transformA,
            edgeRef, edgeInc, outCollisionInfo.edgeNormal, outCollisionInfo.aToBEdgeSeperation))
        {
            //log::debug("aToBEdgeSeperation {} " );
            return;
        }

        manifold.isColliding = true;
    }

    float PhysicsStatics::GetSupportPoint(const std::vector<math::vec3>& vertices, const math::vec3& direction,math::vec3& outVec)
    {
        float currentMaximumSupportPoint = std::numeric_limits<float>::lowest();

        for (const auto& vert : vertices)
        {
            float dotResult = math::dot(direction, vert);

            if (dotResult > currentMaximumSupportPoint)
            {
                currentMaximumSupportPoint = dotResult;
                outVec = vert;
            }
        }

        return currentMaximumSupportPoint;
    }

    bool PhysicsStatics::FindSeperatingAxisByGaussMapEdgeCheck(ConvexCollider* convexA, ConvexCollider* convexB, const math::mat4& transformA,
        const math::mat4& transformB, PointerEncapsulator<HalfEdgeEdge>& refEdge, PointerEncapsulator<HalfEdgeEdge>& incEdge,
        math::vec3& seperatingAxisFound, float& maximumSeperation, bool shouldDebug)
    {
        float currentMinimumSeperation = std::numeric_limits<float>::max();

        math::vec3 centroidDir = transformA * math::vec4(convexA->GetLocalCentroid(), 0);
        math::vec3 positionA = math::vec3(transformA[3]) + centroidDir;

        int facei = 0;
        int facej = 0;

        for (const auto faceA : convexA->GetHalfEdgeFaces())
        {
            //----------------- Get all edges of faceA ------------//
            std::vector<HalfEdgeEdge*> convexAHalfEdges;

            auto lambda = [&convexAHalfEdges](HalfEdgeEdge* edge)
            {
                convexAHalfEdges.push_back(edge);
            };

            faceA->forEachEdge(lambda);
            facej = 0;

            for (const auto faceB : convexB->GetHalfEdgeFaces())
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
                        if (attemptBuildMinkowskiFace(edgeA, edgeB, transformA, transformB))
                        {
                            //get world edge direction
                            math::vec3 edgeADirection = transformA * math::vec4(edgeA->getLocalEdgeDirection(), 0);

                            math::vec3 edgeBDirection = transformB * math::vec4(edgeB->getLocalEdgeDirection(), 0);

                            edgeADirection = math::normalize(edgeADirection);
                            edgeBDirection = math::normalize(edgeBDirection);

                            //get the seperating axis
                            math::vec3 seperatingAxis = math::cross(edgeADirection, edgeBDirection);

                            if (math::epsilonEqual(math::length(seperatingAxis), 0.0f, math::epsilon<float>()))
                            {
                                continue;
                            }

                            seperatingAxis = math::normalize(seperatingAxis);

                            //get world edge position
                            math::vec3 edgeAtransformedPosition = transformA * math::vec4(edgeA->edgePosition, 1);
                            math::vec3 edgeBtransformedPosition = transformB * math::vec4(edgeB->edgePosition, 1);

                            //check if its pointing in the right direction 
                            if (math::dot(seperatingAxis, edgeAtransformedPosition - positionA) < 0)
                            {
                                seperatingAxis = -seperatingAxis;
                            }

                            //check if given edges create a seperating axis
                            float distance = math::dot(seperatingAxis, edgeBtransformedPosition - edgeAtransformedPosition);
                            //log::debug("distance {} , currentMinimumSeperation {}", distance, currentMinimumSeperation);
                            if (distance < currentMinimumSeperation)
                            {
                                refEdge.ptr = edgeA;
                                incEdge.ptr = edgeB;

                                seperatingAxisFound = seperatingAxis;
                                currentMinimumSeperation = distance;
                            }
                        }
                    }
                }
                facej++;
            }
            facei++;
        }

        maximumSeperation = currentMinimumSeperation;
        return currentMinimumSeperation > 0.0f;
    }

    bool PhysicsStatics::DetectConvexSphereCollision(ConvexCollider* convexA, const math::mat4& transformA, math::vec3 sphereWorldPosition, float sphereRadius,
        float& maximumSeperation)
    {
        //-----------------  check if the seperating axis is the line generated between the centroid of the hull and sphereWorldPosition ------------------//

        math::vec3 worldHullCentroid = transformA * math::vec4(convexA->GetLocalCentroid(), 1);
        math::vec3 centroidSeperatingAxis = math::normalize(worldHullCentroid - sphereWorldPosition);

        math::vec3 seperatingPlanePosition = sphereWorldPosition + centroidSeperatingAxis * sphereRadius;

        math::vec3 worldSupportPoint;
        GetSupportPoint(seperatingPlanePosition, -centroidSeperatingAxis, convexA, transformA, worldSupportPoint);

        float seperation = math::dot(worldSupportPoint - seperatingPlanePosition, centroidSeperatingAxis);

        if (seperation > 0.0f)
        {
            maximumSeperation = seperation;
            return false;
        }

        maximumSeperation = std::numeric_limits<float>::lowest();

        //--------------------------------- check if the seperating axis one of the faces of the convex hull ----------------------------------------------//

        for (auto faceA : convexA->GetHalfEdgeFaces())
        {
            math::vec3 worldFaceCentroid = transformA * math::vec4(faceA->centroid, 1);
            math::vec3 worldFaceNormal = math::normalize(transformA * math::vec4(faceA->normal, 0));

            float seperation = PointDistanceToPlane(worldFaceNormal, worldFaceCentroid, seperatingPlanePosition );

            if (seperation > maximumSeperation)
            {
                maximumSeperation = seperation;
            }

            if (seperation > sphereRadius)
            {
                return false;
            }

        }


        return true;
    }

    std::pair< math::vec3, math::vec3> PhysicsStatics::ConstructAABBFromPhysicsComponentWithTransform
    (ecs::component_handle<physicsComponent> physicsComponentToUse,const math::mat4& transform)
    {
        math::vec3 min, max;

        //auto physicsComponent = physicsComponentToUse.read();

        ////get up
        //math::vec3 invTransUp = math::normalize( math::inverse(transform) * math::vec4(math::vec3(0, 1, 0), 0) );
        //max.y = GetPhysicsComponentSupportPointAtDirection(invTransUp, physicsComponent);
        //
        ////get down
        //math::vec3 invTransDown = math::normalize(math::inverse(transform) * math::vec4(math::vec3(0, -1, 0), 0));
        //min.y = GetPhysicsComponentSupportPointAtDirection(invTransUp, physicsComponent);

        ////get right


        ////get left


        ////get forward


        ////get backward


        return std::make_pair(min,max);
    }

    float PhysicsStatics::GetPhysicsComponentSupportPointAtDirection(math::vec3 direction, physicsComponent& physicsComponentToUse)
    {
        float currentMaximumSupportPoint = std::numeric_limits<float>::lowest();

        //std::vector<math::vec3> vertices;
        ////for each vertex list of each collider
        //for (auto collider : *physicsComponentToUse.colliders)
        //{
        //    auto [first,second] = collider->GetminMaxLocalAABB();
        //    vertices.push_back(first);
        //    vertices.push_back(second);
        //}

        //for (const auto& vert : vertices)
        //{
        //    float dotResult = math::dot(direction, vert);

        //    if (dotResult > currentMaximumSupportPoint)
        //    {
        //        currentMaximumSupportPoint = dotResult;
        //    }
        //}

        return currentMaximumSupportPoint;
    }

    std::pair<math::vec3, math::vec3> PhysicsStatics::ConstructAABBFromVertices(const std::vector<math::vec3>& vertices)
    {
        math::vec3 min, max;

        ////up
        //max.y = GetSupportPoint(vertices, math::vec3(0, 1, 0));
        ////down
        //min.y = GetSupportPoint(vertices, math::vec3(0, -1, 0));

        ////right
        //max.x = GetSupportPoint(vertices, math::vec3(1, 0, 0));
        ////left
        //min.x = GetSupportPoint(vertices, math::vec3(-1, 0, 0));
   

        ////forward
        //max.z = GetSupportPoint(vertices, math::vec3(0, 0, 1));
        ////backward
        //min.z = GetSupportPoint(vertices, math::vec3(0, 0, -1));

        return std::make_pair(min,max);
    }

    std::pair<math::vec3, math::vec3> PhysicsStatics::ConstructAABBFromTransformedVertices(const std::vector<math::vec3>& vertices, const math::mat4& transform)
    {
        math::vec3 min, max;
        math::vec3 worldPos = transform[3];

        math::vec3 outVec;
        //up
        math::vec3 invTransUp = math::normalize(math::inverse(transform) * math::vec4(0, 1, 0, 0));
        GetSupportPoint(vertices, invTransUp, outVec);
        max.y = (transform * math::vec4( outVec,1)).y;

        //down
        math::vec3 invTransDown = math::normalize(math::inverse(transform) * math::vec4(0, -1, 0, 0));
        GetSupportPoint(vertices, invTransDown, outVec);
        min.y = (transform * math::vec4(outVec, 1)).y;

        //right
        math::vec3 invTransRight = math::normalize(math::inverse(transform) * math::vec4(1, 0, 0, 0));
        GetSupportPoint(vertices, invTransRight, outVec);
        max.x = (transform * math::vec4(outVec, 1)).x;

        //left
        math::vec3 invTransLeft = math::normalize(math::inverse(transform) * math::vec4(-1, 0, 0, 0));
        GetSupportPoint(vertices, invTransLeft, outVec);
        min.x = (transform * math::vec4(outVec, 1)).x;

        //forward
        math::vec3 invTransForward = math::normalize(math::inverse(transform) * math::vec4(0, 0, 1, 0));
        GetSupportPoint(vertices, invTransForward, outVec);
        max.z = (transform * math::vec4(outVec, 1)).z;

        //backward
        math::vec3 invTransBackward = math::normalize(math::inverse(transform) * math::vec4(0, 0, -1, 0));
        GetSupportPoint(vertices, invTransBackward, outVec);
        min.z = (transform * math::vec4(outVec, 1)).z;

      

        return std::make_pair(min, max);
    }

    std::pair<math::vec3, math::vec3> PhysicsStatics::CombineAABB(const std::pair<math::vec3, math::vec3>& first, const std::pair<math::vec3, math::vec3>& second)
    {
        auto& firstLow = first.first;
        auto& firstHigh = first.second;
        auto& secondLow = second.first;
        auto& secondHigh = second.second;
        math::vec3 lowBounds = secondLow;
        math::vec3 highBounds = secondHigh;
        if (firstLow.x < secondLow.x)   lowBounds.x    = firstLow.x;
        if (firstLow.y < secondLow.y)   lowBounds.y    = firstLow.y;
        if (firstLow.z < secondLow.z)   lowBounds.z    = firstLow.z;
        if (firstHigh.x > secondHigh.x) highBounds.x   = firstHigh.x;
        if (firstHigh.y > secondHigh.y) highBounds.y   = firstHigh.y;
        if (firstHigh.z > secondHigh.z) highBounds.z   = firstHigh.z;

        return std::make_pair(lowBounds, highBounds);
    }

    float PhysicsStatics::FindClosestPointToLineInterpolant(const math::vec3& startPoint, const math::vec3& lineDirection, const math::vec3& pointPosition)
    {
        return ( math::dot(lineDirection,pointPosition) - math::dot(lineDirection,startPoint) ) / math::dot(lineDirection,lineDirection);
    }

    math::vec3 PhysicsStatics::FindClosestPointToLineSegment(const math::vec3& start, const math::vec3& end, const math::vec3& pointPosition)
    {
        float interpolant = FindClosestPointToLineInterpolant(start, end - start, pointPosition);
        interpolant = math::clamp(interpolant, 0.0f, 1.0f);

        return start + (end - start) * interpolant;
    }

    std::shared_ptr<ConvexCollider> PhysicsStatics::GenerateConvexHull(const std::vector<math::vec3>& vertices, int maxDraw, math::mat4 DEBUG_transform )
    {
        auto convexCollider = std::make_shared<ConvexCollider>();
        //Build Initial Hull
        if (!qHBuildInitialHull(vertices, convexCollider,DEBUG_transform))
        {
            return nullptr;
        }

        //For each vertex
            //merge vertex into hull

        return convexCollider;
    }

    bool PhysicsStatics::qHBuildInitialHull(const std::vector<math::vec3>& vertices, std::shared_ptr<ConvexCollider> collider, math::mat4 DEBUG_transform )
    {
        //TODO handle degenerate cases

        //[1] Find the 2 most distant vertices in 'vertices'

        std::vector<math::vec3> supportVertices(6);

        //(1.1) Get support points in -x and x
        GetSupportPoint(vertices, math::vec3(1, 0, 0), supportVertices.at(0));
        GetSupportPoint(vertices, math::vec3(-1, 0, 0), supportVertices.at(1));
                  
        //(1.2) Get support points in -y and y
        GetSupportPoint(vertices, math::vec3(0, 1, 0), supportVertices.at(2));
        GetSupportPoint(vertices, math::vec3(0, -1, 0), supportVertices.at(3));

        //(1.3) Get support points in -z and z
        GetSupportPoint(vertices, math::vec3(0, 0, 1), supportVertices.at(4));
        GetSupportPoint(vertices, math::vec3(0, 0, -1), supportVertices.at(5));

        //(1.4) Find most distant combination
        float mostDistant = std::numeric_limits<float>::lowest();

        math::vec3& firstDistant = supportVertices.at(0);
        math::vec3& secondDistant = supportVertices.at(1);

        //(1.5) Iterate through support vertices to find the combination of vertices that represent the 2 most distant points
        for (int i = 0; i < supportVertices.size(); i++)
        {
            math::vec3& first = supportVertices.at(i);

            for (int j = i+1; j < supportVertices.size()-1; j++)
            {
                math::vec3& second = supportVertices.at(j);

                float currentDistance2 = math::distance2(first, second);

                if (currentDistance2 > mostDistant)
                {
                    mostDistant = currentDistance2;
                    firstDistant = first;
                    secondDistant = second;
                }
            }
        }

        //DEBUG draw most distant
      /*  {
            math::vec3 transformFirst = DEBUG_transform * math::vec4(firstDistant, 1);
            math::vec3 transformSecond = DEBUG_transform * math::vec4(secondDistant, 1);

            debug::drawLine(transformFirst, transformSecond,math::colors::red,5.0f,FLT_MAX,true);
        }*/

        //[2] Find the vertex most distant from the line created by the 2 most distance vertices

        math::vec3 firstToSecond = math::normalize(secondDistant - firstDistant);

        const math::vec3* thirdDistant = nullptr;
        mostDistant = std::numeric_limits<float>::lowest();

        //(2.1) Iterate through 'vertices' to find the vertex most distant from line 
        for (auto& vertex : vertices)
        {
            //(2.1.1) Check if vertex and firstDistant create a line segment parralel to the line segment created by 'firstToSecond'
            float dotResult = math::dot(math::normalize(vertex - firstDistant), firstToSecond);

            if (math::close_enough(dotResult, 1.0f))
            {
                continue;
            }

            //(2.1.2) Find closest point between vertex and line segment
            math::vec3 closestPoint = PhysicsStatics::FindClosestPointToLineSegment(firstDistant, secondDistant, vertex);

            float currentDistance = math::distance2(closestPoint, vertex);

            //(2.1.3) Store vertex if it is further than the current known most distant
            if (currentDistance > mostDistant)
            {
                mostDistant = currentDistance;
                thirdDistant = &vertex;
            }
        }

        if (!thirdDistant)
        {
            return false;
        }

        //DEBUG draw most distant
      /*  {
            math::vec3 transformFirst = DEBUG_transform * math::vec4(*thirdDistant, 1);

            debug::drawLine(transformFirst, transformFirst + math::vec3(0,0.1f,0), math::colors::blue, 5.0f, FLT_MAX, true);
        }*/

        //[3] Create first collider face using that line and the vertex most distant to it

        //(3.1) Initialize Half Edges
        HalfEdgeEdge* firstEdge = new HalfEdgeEdge(firstDistant);
        HalfEdgeEdge* secondEdge = new HalfEdgeEdge(secondDistant);
        HalfEdgeEdge* thirdEdge = new HalfEdgeEdge(*thirdDistant);

        //(3.2) Connect them to each other
        firstEdge->setNextAndPrevEdge(thirdEdge, secondEdge);
        secondEdge->setNextAndPrevEdge(firstEdge, thirdEdge);
        thirdEdge->setNextAndPrevEdge(secondEdge, firstEdge);

        //(3.3) Initialize Half Edge Faces
        HalfEdgeFace* initialFace = new HalfEdgeFace(firstEdge,
            math::normalize(math::cross(secondDistant - firstDistant, *thirdDistant - secondDistant)));

        //(3.4) Add to collider
        collider->GetHalfEdgeFaces().push_back(initialFace);

        //[4] Find the most distant vertex from the plane where the first collider face lies

        //(4.1) Iterate through vertices to the a point to plane check
        mostDistant = std::numeric_limits<float>::lowest();
        const math::vec3* firstEyePoint = nullptr;

        math::vec3 planePosition = initialFace->centroid;
        math::vec3 planeNormal = initialFace->normal;

        for (auto& vertex : vertices)
        {
            float currentDistance =
                math::abs(PhysicsStatics::PointDistanceToPlane(planeNormal, planePosition, vertex));

            if (currentDistance > mostDistant)
            {
                mostDistant = currentDistance;
                firstEyePoint = &vertex;
            }
        }

        if (math::close_enough(mostDistant,0.0f))
        {
            return false;
        }

        //DEBUG draw most distant
        {
          math::vec3 transformFirst = DEBUG_transform * math::vec4(*firstEyePoint, 1);

          debug::drawLine(transformFirst, transformFirst + math::vec3(0,0.1f,0), math::colors::blue, 5.0f, FLT_MAX, true);

        }

        //[5] invert face if distant vertex is in front of face
        float eyePointDistance =
            PhysicsStatics::PointDistanceToPlane(planeNormal, planePosition, *firstEyePoint);

        if (eyePointDistance > 0.0f)
        {
            initialFace->inverse();
        }

        //[5] Create a set of faces connecting the first collider face to the most distant vertex

        return true;
    }

};

