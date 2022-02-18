#pragma once
#include <core/core.hpp>
#include <physics/diviner/data/edge_label.hpp>
namespace legion::physics
{
    struct ContactVertex
    {
        math::vec3 position;
        EdgeLabel label;

        ContactVertex(const math::vec3& pPosition, const EdgeLabel& pLabel) : position(pPosition)
        {
            label = pLabel;
        }

        ContactVertex(const ContactVertex& rhs)
        {
            position = rhs.position;
            label = rhs.label;
        }

        bool operator==( const ContactVertex& rhs)
        {
            return (label.firstEdge == rhs.label.firstEdge) &
                (label.nextEdge == rhs.label.nextEdge);
        }
    };
}
