#pragma once

#include <physics/components/rigidbody.hpp>
#include <core/core.hpp>

namespace args::physics
{
	struct physics_contact
	{
		// these might change later so i wont comment them yet, dont @ me for this.

		ecs::component_handle<rigidbody> rbRefHandle;
		ecs::component_handle<rigidbody> rbIncHandle;

		math::vec3 RefWorldContact;
		math::vec3 IncWorldContact;

		math::vec3 refRBCentroid;
		math::vec3 incRBCentroid;

		math::mat4 refTransform;
		math::mat4 incTransform;

		math::vec3 collisionNormal;

		math::vec3 totaldebugAddedAngular;

		float totalLambda = 0.0f;

		float effectiveMass = 0.0f;

		void resolveContactConstraint(float dt,int i)
		{
			//the idea behind this collision resolution strategy (Sequential Impulse) 
			//is we resolve the collision by giving an impulse towards both 
			//rigidbodies so that the collision would resolve itself in the next frame.

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

			//However, in order to keep this 'shift' in velocity physically based, we must keep in mind the
			//mass and inertia of the rigidbodies involved, therefore we must multiply delta-V with the matrix M^-1

			// delta-V :  M^-1 * (J^T) * lambda

			//where M is the 12x12 matrix
			//[ ma	0	0	0]
			//[ 0	Ia	0	0]
			//[ 0	0	mb	0]
			//[ 0	0	0  Ib]



			//calculate J.V + b
			math::vec3 Ra, Rb, minRaCrossN, RbCrossN;
			calculateJacobianComponents(Ra, minRaCrossN, Rb, RbCrossN);

			math::vec3 va, wa, vb, wb;

			auto RefRB = rbRefHandle.read();
			auto IncRB = rbIncHandle.read();

			va = RefRB.velocity;
			wa = RefRB.angularVelocity;
			vb = IncRB.velocity;
			wb = IncRB.angularVelocity;

			float JVx = math::dot(-collisionNormal, va);
			float JVy = math::dot(minRaCrossN, wa);
			float JVz = math::dot(collisionNormal, vb);
			float JVw = math::dot(RbCrossN, wb);

			float penetration = math::dot(RefWorldContact - IncWorldContact, -collisionNormal);
			float baumgarteConstraint = -penetration * 0.3f * 1/dt;

			float biasFactor = baumgarteConstraint;
			float minJV = -(JVx + JVy + JVz + JVw) + biasFactor;
			
			float foundLambda = minJV / effectiveMass;

			float oldTotalLambda = totalLambda;
			totalLambda += foundLambda;

			totalLambda = math::clamp(totalLambda, 0.0f, std::numeric_limits<float>::max());

			float lambdaApplied = totalLambda - oldTotalLambda;

			/*log::debug("/////lambda calculation/////");
			log::debug("JVX {}", JVx);
			log::debug("JVY {}", JVy);
			log::debug("JVZ {}", JVz);
			log::debug("JVW {}", JVw);
			log::debug("/////JV found/////");
			log::debug("-jv found {} ", minJV);
			log::debug("effective mass {} ", effectiveMass);
			log::debug("/////lambda found/////");
			log::debug("lambda found {} ", foundLambda);
			log::debug("lambda applied {} ", lambdaApplied);
			log::debug("totalLambda {} ", totalLambda);
			log::debug("/////current velocities/////");
			log::debug("VA {}", math::to_string(va));
			log::debug("WA {}", math::to_string(wa));
			log::debug("------------------------");
			log::debug("VB {}", math::to_string(vb));
			log::debug("WB {}", math::to_string(wb));*/

			ApplyImpulse(collisionNormal, lambdaApplied,
				Ra, Rb);
			//calculate lambda 

			//clamp lambda

		}

		void preCalculateEffectiveMass()
		{
			collisionNormal = math::normalize(collisionNormal);
			//calculate J M^-1 J^T

			//J: [-n(-Ra x n)	n	(Rb x n)]

			//calculate Ra and Rb

			math::vec3 Ra, Rb, minRaCrossN, RbCrossN;
			calculateJacobianComponents(Ra, minRaCrossN, Rb, RbCrossN);

			float maUnit = rbRefHandle ? rbRefHandle.read().inverseMass : 0.0f;
			float mbUnit = rbIncHandle ? rbIncHandle.read().inverseMass : 0.0f;

			//calculate M-1
			math::mat3 ma = math::mat3(maUnit);
			math::mat3 Ia = rbRefHandle ? rbRefHandle.read().globalInverseInertiaTensor : math::mat3(0.0f);
			math::mat3 mb = math::mat3(mbUnit);
			math::mat3 Ib = rbIncHandle ? rbIncHandle.read().globalInverseInertiaTensor : math::mat3(0.0f);

			//calculate M^-1 J^T
			math::vec3 jx = ma * -collisionNormal;
			math::vec3 jy = Ia * minRaCrossN;
			math::vec3 jz = mb * collisionNormal;
			math::vec3 jw = Ib * RbCrossN;


			//calculate effective mass
			float efMx = math::dot(-collisionNormal, jx);
			float efMy = math::dot(minRaCrossN, jy);
			float efMz = math::dot(collisionNormal, jz);
			float efMw = math::dot(RbCrossN, jw);

			effectiveMass = efMx + efMy + efMz + efMw;

			//assert(efMx >= 0.0f);
			//assert(efMy >= 0.0f);
			//assert(efMz >= 0.0f);
			//assert(efMw >= 0.0f);
		}

		

		void ApplyImpulse(const math::vec3 normal, const float lambda, 
			const math::vec3 ra, const math::vec3 rb)
		{
			/*log::debug("-----------ApplyImpulse------------------------");
			log::debug("lambda applied {} ", lambda);*/

			math::vec3 linearImpulse = normal * lambda;

			math::vec3 torqueDirectionA = math::cross(-ra, normal);
			math::vec3 torqueDirectionB = math::cross(rb, normal);

			math::vec3 angularImpulseA = torqueDirectionA * lambda;
			math::vec3 angularImpulseB = torqueDirectionB * lambda;

			if (rbRefHandle)
			{
				auto refRb = rbRefHandle.read();

				refRb.velocity += -linearImpulse * refRb.inverseMass;
				refRb.angularVelocity += refRb.globalInverseInertiaTensor * angularImpulseA;

				rbRefHandle.write(refRb);

			}

			if (rbIncHandle)
			{
				auto incRb = rbIncHandle.read();

				incRb.velocity += linearImpulse * incRb.inverseMass;
				auto addedAngular = incRb.globalInverseInertiaTensor * angularImpulseB;
				incRb.angularVelocity += addedAngular;

				//log::debug("-----------rbInc after collision -------------");
				//log::debug("//////END OF CONSTRAINT////////////");
				//log::debug("afr incRb.angularVelocity {} ", math::to_string(incRb.angularVelocity));
				//log::debug("afr incRb.velocity {} ", math::to_string(incRb.velocity));
				//log::debug("addedAngular {} ", math::to_string(incRb.angularVelocity));

				totaldebugAddedAngular = incRb.globalInverseInertiaTensor * torqueDirectionB * totalLambda;
				
				rbIncHandle.write(incRb);

			}

		}


		void resolveFrictionConstraint()
		{

		}



		void calculateJacobianComponents(math::vec3& Ra, math::vec3& minRaCrossN, math::vec3& Rb, math::vec3& RbCrossN)
		{
			Ra = RefWorldContact - refRBCentroid;
			Rb = IncWorldContact - incRBCentroid;

			minRaCrossN = math::cross(-Ra, collisionNormal);
			RbCrossN = math::cross(Rb, collisionNormal);
		}

		void logRigidbodyState()
		{
			//log::debug("//--------logRigidbodyState----------//");
			auto incRb = rbIncHandle.read();
			auto refRb = rbRefHandle.read();
	/*		log::debug("incRb.velocity {} ", math::to_string(incRb.velocity));
			log::debug("incRb.angularVelocity {} ", math::to_string(incRb.angularVelocity));
			log::debug("////////////////////////////");*/
		}
	};

	

}
