#pragma once
#include <core/core.hpp>
#include <physics/diviner/data/convergence_identifier.hpp>
#include <physics/diviner/data/edge_label.hpp>
namespace legion::physics
{
    class ConvexConvergenceIdentifier : public ConvergenceIdentifier
    {
    public:

        ConvexConvergenceIdentifier(const EdgeLabel& label, float pTotalLambda,
            float pTangent1Lambda, float pTangent2Lambda, int pRefColliderId)
            : ConvergenceIdentifier(label,pTotalLambda, pTangent1Lambda, pTangent2Lambda, pRefColliderId)
        {

        }

       

        bool IsEqual(physics_contact& contact) override
        {
            return label == contact.label;
        }



    private:
 

    };
}
