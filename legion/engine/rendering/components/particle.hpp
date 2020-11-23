#pragma once
#include <core/core.hpp>

namespace legion::rendering
{
   struct particle
   {
       float lifeTime;
       math::vec3 velocity;
   };
}
