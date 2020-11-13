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
            : ConverganceIdentifier(label,pTotalLambda, pTangent1Lambda, pTangent2Lambda, pRefColliderId)
        {

        }

       

        bool IsEqual(physics_contact& contact) override
        {
            return label == contact.label;
        }



    private:
 

    };
}
