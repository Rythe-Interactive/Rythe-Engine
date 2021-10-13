#pragma once

#include <core/core.hpp>

namespace legion::physics::constants
{
    static math::vec3 gravity = math::vec3(0, -9.81, 0);

    static constexpr int contactSolverIterationCount = 6;

    static constexpr int frictionSolverIterationCount = 4;

    static constexpr float faceToFacePenetrationBias = 0.005f;

    static constexpr float faceToEdgePenetrationBias = 0.05f;

    static constexpr float baumgarteCoefficient = 0.50f;

    static constexpr float baumgarteSlop = 0.01f;

    static constexpr float restitutionSlop = 0.5f;

    static constexpr float contactOffset = 0.01f;

    static constexpr float sutherlandHodgmanClippingThreshold = 0.01f;

    static constexpr bool applyWarmStarting = true;

    static constexpr float polygonItersectionEpsilon = 0.01f;

    static constexpr float polygonSplitterEpsilon = 0.01f;
}
