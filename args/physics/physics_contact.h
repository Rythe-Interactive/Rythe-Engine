#pragma once

#include <physics/components/rigidbody.hpp>
#include <core/core.hpp>

namespace args::physics
{
	struct physics_contact
	{
		// these might change later so i wont comment them yet, dont @ me for this.

		ecs::component_handle<rigidbody> rbRef;
		ecs::component_handle<rigidbody> rbInc;

		math::vec3 worldContactRef;
		math::vec3 worldContactInc;

		math::vec3 refWorldColliderCentroid;
		math::vec3 incWorldColliderCentroid;

		math::mat4 refTransform;
		math::mat4 incTransform;

		math::vec3 collisionNormal;



		float totalLambda = 0.0f;

		void resolveContactConstraint()
		{
			//the idea behind this collision resolution strategy is we resolve the collision by giving an impulse towards both 
			//rigidbodies so that the collision would resolve itself in the next frame.
			//We calculate this value by first getting the position constraint

			// C : (Pa - Pb).n = 0

			//Pa is the contact point of rigidbodyA and Pb is the contact point of rigidbodyB
			//vector from the center of rigidbodyB towards its contact point. And n is the collision normal

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

			// delta-V :  (M^-1) * (J^T) * lambda

			//where M is the 12x12 matrix
			//[ ma	0	0	0]
			//[ 0	Ia	0	0]
			//[ 0	0	mb	0]
			//[ 0	0	0  Ib]




			//calculate J.V + b

			//calculate lambda 

			//clamp lambda

		}

		void preCalculateEffectiveMass()
		{
			//calculate J M J^T
		}

		void resolveFrictionConstraint()
		{

		}

		void applyImpulse(float lambda)
		{

		}


	};
}
