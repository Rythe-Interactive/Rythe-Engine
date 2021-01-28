#pragma once

#include <physics/components/rigidbody.hpp>
#include <core/core.hpp>
#include <physics/data/identifier.hpp>
#include <physics/data/edge_label.hpp>



namespace legion::physics
{
    class PhysicsCollider;

    struct physics_contact
    {
        PhysicsCollider* refCollider;

        EdgeLabel label;

        rigidbody* rbRef;
        rigidbody* rbInc;

        math::vec3 RefWorldContact;
        math::vec3 IncWorldContact;

        math::vec3 refRBCentroid;
        math::vec3 incRBCentroid;

        math::mat4 refTransform;
        math::mat4 incTransform;

        math::vec3 collisionNormal;
        math::vec3 tangentNormal1;
        math::vec3 tangentNormal2;

        float totalLambda = 0.0f;
        float tangent1Lambda = 0.0f;
        float tangent2Lambda = 0.0f;

        float effectiveMass = 0.0f;

        float tangent1EffectiveMass = 0.0f;
        float tangent2EffectiveMass = 0.0f;

        int contactCount = 0;

        void ApplyWarmStarting()
        {
            OPTICK_EVENT();

            math::vec3 Ra = RefWorldContact - refRBCentroid;
            math::vec3 Rb = IncWorldContact - incRBCentroid;

            ApplyImpulse(collisionNormal, totalLambda, Ra, Rb);
            ApplyImpulse(tangentNormal1, tangent1Lambda, Ra, Rb);
            ApplyImpulse(tangentNormal2, tangent2Lambda, Ra, Rb);
        }

        /* @brief Calculate a certain linear and angular impulse that will resolve the collision
        */
        void resolveContactConstraint(float dt, int i)
        {
            OPTICK_EVENT();

            //the idea behind this collision resolution strategy (Sequential Impulses) 
            //is we resolve the collision by giving an impulse towards both 
            //rigidbodies so that the collision would resolve itself in the next time step.

            //We calculate this value by first getting the position constraint

            // C : (Pa - Pb).n = 0

            //Pa is the vector from the centroid of rigidbodyA to the contact point of rigidbodyA
            //and Pb is the vector from the centroid of rigidbodyB to the contact point of rigidbodyB
            //and n is the collision normal

            //Next, in order to apply the aformentioned impulse.We must get the velocity constraint. In order to do so,
            //we must find the time deriavative of our position constraint using the product rule of differentiation.

            //Cdot: [(Vb + Wb x Rb - (Va + Wa x Ra))] .n + (Pb - Pa) .[Na x Wa]

            //where Ra is the vector from the center of rigidbodyA towards Pa and Rb 
            //is the vector from the center of rigidbodyB towards Pb. Where every 'V' stands for velocity and every
            //'W' stands for angular velocity

            //the position part of this constraint can be ignored so we get:
            //Cdot: [(Vb + Wb x Rb - (Va + Wa x Ra))] .n

            //Using the triple product identity, we can isolate the angular and linear velocities so we get

            //Cdot: J.V

            //where J is a 1x12 matrix [ -n		(-Ra x n)	n	(Rb x n) ] 
            //and V is a 12x1 matrix  ([ -Va	(-Wa)		Vb  (Wb)	 ])^T

            //By changing the form of the constraint we now see that J dictates the rate of change of V 
            //(similar to how the function of a line is y = mx + b)
            //In other words, J is the jacobian matrix of V. This means that if we are looking for a certain delta-V
            //that would resolve the collision it would be equal to:

            // delta-V :  (J^T) * lambda

            //Where lambda is a single value that represents the scalar value of the linear and angular impulse

            //However, in order to keep this 'shift' in velocity physically based, we must keep in mind the
            //mass and inertia of the rigidbodies involved, therefore we must multiply delta-V with the matrix M^-1

            // delta-V :  M^-1 * (J^T) * lambda

            //where M is the 12x12 matrix
            //[ ma	0	0	0]
            //[ 0	Ia	0	0]
            //[ 0	0	mb	0]
            //[ 0	0	0  Ib]

            //where ma is a 3x3 identity matrix scaled by the mass of the rigidbodyA,
            //mb is a 3x3 identity matrix scaled by the mass of the rigidbodyB,
            //where Ia is the 3x3 matrix inertia tensor of the rigidbodyA,
            //where Ib is the 3x3 matrix inertia tensor of the rigidbodyB,

            //calculate J.V + b
            math::vec3 Ra, Rb, minRaCrossN, RbCrossN;
            calculateJacobianComponents(collisionNormal, Ra, minRaCrossN, Rb, RbCrossN);

            math::vec3 va, wa, vb, wb;

            va = rbRef ? rbRef->velocity : math::vec3::zero;
            wa = rbRef ? rbRef->angularVelocity : math::vec3::zero;
            vb = rbInc ? rbInc->velocity : math::vec3::zero;
            wb = rbInc ? rbInc->angularVelocity : math::vec3::zero;

            float JVx = math::dot(-collisionNormal, va);
            float JVy = math::dot(minRaCrossN, wa);
            float JVz = math::dot(collisionNormal, vb);
            float JVw = math::dot(RbCrossN, wb);

            float minJV = -(JVx + JVy + JVz + JVw);


            //log::debug("CALCULATE");

            //-------------------------- Positional Constraint ----------------------------------//

            //resolve the position violation by adding it into the lambda
            float penetration = math::dot(RefWorldContact - IncWorldContact, -collisionNormal);

            //but allow some penetration for the sake of stability
            penetration = math::min(penetration + physics::constants::baumgarteSlop, 0.0f);

            float baumgarteConstraint = -penetration * physics::constants::baumgarteCoefficient * 1 / dt;

            //-------------------------- Restitution Constraint ----------------------------------//

            //calculate restitution between the 2 bodies
            float restCoeff = rigidbody::calculateRestitution(rbRef? rbRef->restitution : 0.3f, rbInc? rbInc->restitution : 0.3f);

            math::vec3 minWaCrossRa = math::cross(-wa, Ra);
            math::vec3 WbCrossRb = math::cross(wb, Rb);

            //restitution is based on the relative velocities of the 2 rigidbodies
            float dotResult = math::dot((-va + minWaCrossRa + vb + WbCrossRb), collisionNormal);

            float restitutionConstraint = dotResult * restCoeff;

            restitutionConstraint = math::max(restitutionConstraint - physics::constants::restitutionSlop, 0.0f);

            //-------------------------- Velocity Constraint ----------------------------------//



            float biasFactor = baumgarteConstraint + restitutionConstraint;

            float foundLambda = (minJV + biasFactor) / effectiveMass;

            float oldTotalLambda = totalLambda;
            totalLambda += foundLambda;

            totalLambda = math::clamp(totalLambda, 0.0f, std::numeric_limits<float>::max());

            float lambdaApplied = totalLambda - oldTotalLambda;

            ApplyImpulse(collisionNormal, lambdaApplied,
                Ra, Rb);

        }

        void resolveFrictionConstraint()
        {
            OPTICK_EVENT();

            float frictionCoeff = rigidbody::calculateFriction(rbRef ? rbRef->friction : 0.3f, rbInc ? rbInc->friction : 0.3f);
            float frictionConstraint = totalLambda * frictionCoeff;

            math::vec3 Ra = RefWorldContact - refRBCentroid;
            math::vec3 Rb = IncWorldContact - incRBCentroid;

            //calculate friction constraint for tangent1
            {
                float tangent1MinJV = calculateJVConstraint(tangentNormal1);

                float foundLambda = tangent1MinJV / tangent1EffectiveMass;

                float oldTotalLambda = tangent1Lambda;
                tangent1Lambda += foundLambda;

                tangent1Lambda = math::clamp(tangent1Lambda, -frictionConstraint, frictionConstraint);

                float lambdaApplied = tangent1Lambda - oldTotalLambda;

                ApplyImpulse(tangentNormal1, lambdaApplied,
                    Ra, Rb);

            }

            //calculate friction constraint for tangent1
            {
                float tangent2MinJV = calculateJVConstraint(tangentNormal2);

                float foundLambda = tangent2MinJV / tangent2EffectiveMass;

                float oldTotalLambda = tangent2Lambda;
                tangent2Lambda += foundLambda;

                tangent2Lambda = math::clamp(tangent2Lambda, -frictionConstraint, frictionConstraint);

                float lambdaApplied = tangent2Lambda - oldTotalLambda;

                ApplyImpulse(tangentNormal2, lambdaApplied,
                    Ra, Rb);
            }
        }

        /* @brief Given the equation to calculate lambda -(J.V)/(J * M^-1 * J^T),
        * precalculates (J * M^-1 * J^T).
        */
        void preCalculateEffectiveMass()
        {
            OPTICK_EVENT();
            //calculate tangent vectors

            //--------------------------- pre calculate contact constraint effective mass -----------------------------------//

            tangentNormal1 = math::cross(collisionNormal, math::vec3(1, 0, 0));

            if (math::epsilonEqual(math::length(tangentNormal1), 0.0f, 0.01f))
            {
                tangentNormal1 = math::cross(collisionNormal, math::vec3(0, 1, 0));
            }

            tangentNormal1 = math::normalize(tangentNormal1);

            tangentNormal2 = math::normalize(math::cross(collisionNormal, tangentNormal1));

            effectiveMass = calculateEffectiveMassOnNormal(collisionNormal);
            tangent1EffectiveMass = calculateEffectiveMassOnNormal(tangentNormal1);
            tangent2EffectiveMass = calculateEffectiveMassOnNormal(tangentNormal2);
        }

        float calculateJVConstraint(const math::vec3& normal)
        {
            math::vec3 Ra, Rb, minRaCrossN, RbCrossN;
            calculateJacobianComponents(normal, Ra, minRaCrossN, Rb, RbCrossN);

            math::vec3 va, wa, vb, wb;

            va = rbRef ? rbRef->velocity : math::vec3::zero;
            wa = rbRef ? rbRef->angularVelocity : math::vec3::zero;
            vb = rbInc ? rbInc->velocity : math::vec3::zero;
            wb = rbInc ? rbInc->angularVelocity : math::vec3::zero;

            float JVx = math::dot(-normal, va);
            float JVy = math::dot(minRaCrossN, wa);
            float JVz = math::dot(normal, vb);
            float JVw = math::dot(RbCrossN, wb);

            return -(JVx + JVy + JVz + JVw);
        }

        float calculateEffectiveMassOnNormal(math::vec3& normal)
        {
            math::vec3 Ra, Rb, minRaCrossN, RbCrossN;
            calculateJacobianComponents(normal, Ra, minRaCrossN, Rb, RbCrossN);

            float maUnit = rbRef ? rbRef->inverseMass : 0.0f;
            float mbUnit = rbInc ? rbInc->inverseMass : 0.0f;

            //calculate M-1
            math::mat3 ma = math::mat3(maUnit);
            math::mat3 Ia = rbRef ? rbRef->globalInverseInertiaTensor : math::mat3(1.f);
            math::mat3 mb = math::mat3(mbUnit);                                
            math::mat3 Ib = rbInc ? rbInc->globalInverseInertiaTensor : math::mat3(1.f);

            //calculate M^-1 J^T
            math::vec3 jx = ma * -normal;
            math::vec3 jy = Ia * minRaCrossN;
            math::vec3 jz = mb * normal;
            math::vec3 jw = Ib * RbCrossN;

            //calculate effective mass
            float efMx = math::dot(-normal, jx);
            float efMy = math::dot(minRaCrossN, jy);
            float efMz = math::dot(normal, jz);
            float efMw = math::dot(RbCrossN, jw);

            return efMx + efMy + efMz + efMw;

        }

        /* @brief Given a normal indicating the impulse direction, the vectors ra and rb that indicate the contact vectors,
        * and a lambda that indicates the scalar value of the impulse, applies impulses to the colliding rigidbodies
        */
        void ApplyImpulse(const math::vec3& normal, const float lambda,
            const math::vec3& ra, const math::vec3& rb)
        {

            math::vec3 linearImpulse = normal * lambda;

            math::vec3 torqueDirectionA = math::cross(-ra, normal);
            math::vec3 torqueDirectionB = math::cross(rb, normal);

            math::vec3 angularImpulseA = torqueDirectionA * lambda;      
            math::vec3 angularImpulseB = torqueDirectionB * lambda;

            if (rbRef)
            {
                rbRef->velocity += -linearImpulse * rbRef->inverseMass;
                rbRef->angularVelocity += rbRef->globalInverseInertiaTensor * angularImpulseA;
            }

            if (rbInc)
            {
                rbInc->velocity += linearImpulse * rbInc->inverseMass;
                auto addedAngular = rbInc->globalInverseInertiaTensor * angularImpulseB;
                rbInc->angularVelocity += addedAngular;
            }
        }

        void calculateJacobianComponents(const math::vec3 normal,
            math::vec3& Ra, math::vec3& minRaCrossN, math::vec3& Rb, math::vec3& RbCrossN)
        {
            Ra = RefWorldContact - refRBCentroid;
            Rb = IncWorldContact - incRBCentroid;

            minRaCrossN = math::cross(-Ra, normal);
            RbCrossN = math::cross(Rb, normal);
        }



        void logRigidbodyState()
        {
            log::debug("//--------logRigidbodyState----------//");
            log::debug("rbInc->velocity {} ", math::to_string(rbInc->velocity));
            log::debug("rbInc->angularVelocity {} ", math::to_string(rbInc->angularVelocity));

            log::debug("collisionNormal {} ", math::to_string(collisionNormal));
            log::debug("lambda {} ", totalLambda);
            log::debug("////////////////////////////");
        }
    };



}
