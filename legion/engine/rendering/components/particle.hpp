#pragma once
#include <core/core.hpp>

namespace legion::rendering
{
   /**
    * @brief Particle component holding both the lifeTime and velocity of the particle.
    */
   struct particle
   {
       float lifeTime;
       velocity particleVelocity;
   };
}
