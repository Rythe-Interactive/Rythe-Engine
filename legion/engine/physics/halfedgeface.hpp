#pragma once
#include <core/core.hpp>

namespace legion::physics
{
	struct HalfEdgeEdge;

	struct HalfEdgeFace
	{
		HalfEdgeEdge* startEdge;
		math::vec3 normal;
		math::vec3 centroid;
        
		HalfEdgeFace(HalfEdgeEdge* newStartEdge, math::vec3 newNormal);

		/** @brief given a function that takes in a HalfEdgeEdge*, 
		* executes the function on each edge connected to 'startEdge'
		*/
		void forEachEdge(legion::core::delegate< void(HalfEdgeEdge*)> functionToExecute);

        /**@brief Inverses the face
         * The edges will be stored in reverse and therefore the normal will point in the other direction
         * The normal will be changed 
         */
        void inverse();

        /**@brief Tests if this face is convex in regard to the passed face
         * @param other - the other plane
         * @return Returns true when convex, false if coplaner or concave
         */
        bool testConvexity(const HalfEdgeFace& other) const;

        /**@brief Makes the normals for this faces point to the convex side of the plane in regard to each other
         * @return Returns true when the face has been made convex, false when it was already convex
         */
        bool makeNormalsConvexWithFace(HalfEdgeFace& other);

        /**@brief Tests if the the two planes are convex in regard to each other
         * @return true when convex, false if coplaner or concave
         */
        static bool testConvexity(const HalfEdgeFace& first, const HalfEdgeFace& second);

        /**@brief Makes the normals for both faces point to the convex side of the plane in regard to each other
         * @return Returns true when one of the faces or both have been made convex, false when they were already convex
         */
        static bool makeNormalsConvexWithFace(HalfEdgeFace& first, HalfEdgeFace& second);
		
		~HalfEdgeFace();
	};
}
