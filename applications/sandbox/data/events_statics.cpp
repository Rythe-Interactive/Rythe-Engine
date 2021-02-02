#include "decal_event.hpp"
#include "particle_event.hpp"
#include "explode_event.hpp"

namespace ext::evt
{
    std::vector<legion::ecs::entity_handle> g_er_entities;
    std::vector<legion::ecs::entity_handle> g_dr_entities;
    std::vector<legion::ecs::entity_handle> g_pr_entities;
}
