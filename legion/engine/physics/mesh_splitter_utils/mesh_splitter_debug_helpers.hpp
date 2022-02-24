#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter_typedefs.hpp>

namespace legion::physics
{
    struct BoundaryEdgeInfo
    {
        math::float3 worldNormal;
        std::pair<math::float3, math::float3> intersectionPoints;
        std::pair<math::float3, math::float3> finalSortingDirection;
        std::vector<meshHalfEdgePtr> boundaryEdges;
        math::color drawColor;

        math::float3 nextSupport;
        math::float3 prevSupport;
        math::float3 intersectionEdge;
        math::float3 base;

    };

    //struct 

    struct MeshSplitterDebugHelper
    {
        std::pair<math::float3, math::float3> cuttingSetting;
        math::float4x4 DEBUG_transform;
        std::vector<math::float3> intersectionsPolygons;
        std::vector<math::float3> nonIntersectionPolygons;
        std::vector< std::vector<math::float3>> intersectionIslands;

        std::vector<BoundaryEdgeInfo> boundaryEdgesForPolygon;

        std::array< math::color, 5> colors =
        { math::colors::red,
            math::colors::cyan,
            math::colors::green,
            math::colors::magenta,
            math::colors::yellow
        };

        //std::vector

        int polygonCount;

        int polygonToDisplay = 0;

    };

    struct SplitterIntersectionInfo
    {
        SplitterIntersectionInfo(const math::float3& pStartIntersection, const math::float3& pStartToEndPosition,
            const math::float2& pStartIntersectionUV, const math::float2& pStartToEndUV) :

            startIntersectionPosition(pStartIntersection),
            startToEndPosition(pStartToEndPosition),
            startIntersectionUV(pStartIntersectionUV),
            startToEndUV(pStartToEndUV)
        {

        }

        auto GetIntersectionData() const
        {
            return std::make_tuple(startIntersectionPosition, startToEndPosition, startIntersectionUV, startToEndUV);
        }

        math::float3 startIntersectionPosition;
        math::float3 startToEndPosition;

        math::float2 startIntersectionUV;
        math::float2 startToEndUV;


    };

}

