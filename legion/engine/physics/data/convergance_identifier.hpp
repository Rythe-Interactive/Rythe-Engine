#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    class ConvexConverganceIdentifier;

    class ConverganceIdentifier
    {
    public:

        ConverganceIdentifier(float pTotalLambda,
            float pTangent1Lambda, float pTangent2Lambda,int pRefColliderId)
            : totalLambda(pTotalLambda),tangent1Lambda(pTangent1Lambda),
            tangent2Lambda(pTangent2Lambda),refColliderID(pRefColliderId)
        {

        }

        float totalLambda = 0.0f;
        float tangent1Lambda = 0.0f;
        float tangent2Lambda = 0.0f;

        int refColliderID = -1;

        virtual bool IsEqual(ConverganceIdentifier* identifier) = 0;

        virtual bool IsEqualWith(ConvexConverganceIdentifier* identifier) = 0;


    };
}
