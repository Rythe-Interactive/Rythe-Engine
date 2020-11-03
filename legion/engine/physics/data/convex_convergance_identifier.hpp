#pragma once
#include <core/core.hpp>
#include <physics/data/convergance_identifier.hpp>
#include <physics/data/edge_label.hpp>
namespace legion::physics
{
    class ConvexConverganceIdentifier : public ConverganceIdentifier
    {
    public:

        ConvexConverganceIdentifier(const EdgeLabel& label, float pTotalLambda,
            float pTangent1Lambda, float pTangent2Lambda, int pRefColliderId)
            : ConverganceIdentifier(pTotalLambda, pTangent1Lambda, pTangent2Lambda, pRefColliderId)
        {

        }

        bool IsEqual(ConverganceIdentifier* identifier) override
        {
            return identifier->IsEqualWith(this);
        }

        bool IsEqualWith(ConvexConverganceIdentifier* identifier) override
        {
            return label == identifier->label;
        }

    private:
        EdgeLabel label;

    };
}
