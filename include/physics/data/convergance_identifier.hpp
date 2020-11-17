#pragma once
#include <core/core.hpp>
#include <physics/data/edge_label.hpp>
#include <physics/physics_contact.hpp>

namespace legion::physics
{
    class ConvexConverganceIdentifier;

    class ConverganceIdentifier
    {
    public:

        ConverganceIdentifier(const EdgeLabel& plabel,float pTotalLambda,
            float pTangent1Lambda, float pTangent2Lambda,int pRefColliderId)
            :  totalLambda(pTotalLambda),tangent1Lambda(pTangent1Lambda),
            tangent2Lambda(pTangent2Lambda),refColliderID(pRefColliderId)
        {
            this->label = plabel;
        }

        float totalLambda = 0.0f;
        float tangent1Lambda = 0.0f;
        float tangent2Lambda = 0.0f;
        EdgeLabel label;
        int refColliderID = -1;

        void CopyLambdasToContact(physics_contact& contact)
        {
            contact.totalLambda = totalLambda;
            contact.tangent1Lambda = tangent1Lambda;
            contact.tangent2Lambda = tangent2Lambda;
        }

        virtual bool IsEqual(physics_contact& contact) { return false; };
    };
}
