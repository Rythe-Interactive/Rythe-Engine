#pragma once
#include <core/core.hpp>
#include <physics/diviner/halfedgeedge.hpp>


namespace legion::physics
{
    struct ColliderFaceToVert;

    struct HalfEdgeFace
    {
        math::vec3 normal;
        math::vec3 centroid;

        HalfEdgeEdge* startEdge = nullptr;
        ColliderFaceToVert* faceToVert = nullptr; 
        
        HalfEdgeFace(HalfEdgeEdge* newStartEdge, math::vec3 newNormal);

        /**@brief Given that the face has a startEdge,a normal, and a centroid,
         * initializes the face so that it can be used for collision detection in a convex hull
         */
        void initializeFace();

        /**@brief Calculates the sum of the width and height of the rectangle that envelops this face
         */
        float calculateFaceExtents();

        /** @brief given a function that takes in a HalfEdgeEdge*, 
        * executes the function on each edge connected to 'startEdge'
        */
        void forEachEdge(legion::core::delegate< void(HalfEdgeEdge*)> functionToExecute,
            legion::core::delegate <HalfEdgeEdge* (HalfEdgeEdge*)> getNextEdge = [](HalfEdgeEdge* current) { return current->nextEdge; });

        void forEachEdgeReverse(legion::core::delegate< void(HalfEdgeEdge*)> functionToExecute);

        /**@brief Inverses the face
         * The edges will be stored in reverse and therefore the normal will point in the other direction
         * The normal will be changed 
         */
        void inverse();

        void DEBUG_DrawFace(const math::mat4& transform, const math::color& debugColor,  float time = 20.0f);

        void DEBUG_DirectionDrawFace(const math::mat4& transform, const math::color& debugColor, float time = 20.0f);

        ~HalfEdgeFace();
    };
}
