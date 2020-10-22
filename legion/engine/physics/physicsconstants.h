#pragma once

#include <core/core.hpp>

namespace legion::physics::constants
{
	static math::vec3 gravity = math::vec3(0, -9.81, 0);

	static constexpr int contactSolverIterationCount = 10;

	static constexpr int frictionSolverIterationCount = 4;
}
