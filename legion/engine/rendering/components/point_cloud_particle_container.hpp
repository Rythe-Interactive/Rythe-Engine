#pragma once
#include<core/core.hpp>

namespace legion::rendering
{

    struct point_cloud_particle_container
    {
    public:
        std::vector<ecs::entity> livingParticles;
        std::vector<ecs::entity> deadParticles;
        std::vector<math::color> colorBufferData;
    };

}
