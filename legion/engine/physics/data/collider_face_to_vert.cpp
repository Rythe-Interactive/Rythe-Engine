#include <physics/data/collider_face_to_vert.hpp>
#include <physics/physics_statics.hpp>

namespace legion::physics
{
    std::pair<math::vec3, float> legion::physics::ColliderFaceToVert::GetFurthestOutsideVert() const
    {
        const math::vec3& faceCentroid = face->centroid;
        const math::vec3& faceNormal = face->normal;

        float largestDist = std::numeric_limits<float>::lowest();
        const math::vec3* mostDistantVert = nullptr;

        for (const math::vec3& vert : outsideVerts)
        {
            float distance = PhysicsStatics::PointDistanceToPlane(faceNormal,faceCentroid,vert);

            if (distance > largestDist)
            {
                largestDist = distance;
                mostDistantVert = &vert;
            }
        }

        return std::make_pair(*mostDistantVert,largestDist);
    }
}

