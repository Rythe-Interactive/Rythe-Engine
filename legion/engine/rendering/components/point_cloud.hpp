#pragma once
#include<core/core.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <application/application.hpp>


#include <core/compute/context.hpp>
#include <core/compute/kernel.hpp>



namespace legion::rendering
{

    struct point_cloud
    {
    public:
        point_cloud(mesh_handle newMesh, transform newTrans, material_handle mat, uint maxPoints = 100, float radius = 0.2f)
        {
            m_mesh = newMesh;
            m_trans = newTrans;
            m_samplesPerTriangle = calculateSamplesPerTriangle(maxPoints);
            m_pointRadius = radius;
            m_Material = mat;
        }
        point_cloud() = default;

    private:
        friend class PointCloudGeneration;

        bool m_hasBeenGenerated = false;
        float m_pointRadius;


        transform m_trans;
        mesh_handle m_mesh;
        uint m_samplesPerTriangle;
        material_handle m_Material;

        uint calculateSamplesPerTriangle(uint maxPoints)
        {
            if (!m_mesh) return 0;
            auto mesh = m_mesh.get();
            auto indices = mesh.second.indices;
            uint triangleCount = indices.size() / 3;
            return maxPoints / triangleCount;
        }
    };



}
