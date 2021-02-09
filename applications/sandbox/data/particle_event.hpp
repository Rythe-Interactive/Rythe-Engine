#pragma once
#include "animation_events.hpp"
#include <rendering/util/gui.hpp>

namespace ext::evt
{
    struct particle_receiver { std::string name; };
    extern std::vector<legion::ecs::entity_handle> g_pr_entities;

    struct particle_event : animation_event<particle_event>
    {
        static bool onGUI(legion::id_type id, animation_event_base* ev) {
            using namespace imgui;
            base::Text("Particle Event");

            std::string affectedStr = ev->get_param("_affectedEntity");

            base::Text("Affected Entity");
            base::SameLine();
            if (base::BeginCombo("##TypePAEVT", affectedStr.data())) {

                for (const legion::ecs::entity_handle& handle : g_pr_entities)
                {
                    auto er = handle.read_component<particle_receiver>();
                    //we query the database for entries and display them in a combobox, which makes for a pretty useful chooser
                    if (ImGui::Selectable(std::string(er.name).c_str(), er.name == affectedStr))
                    {
                        affectedStr = er.name;
                    }
                }
                base::EndCombo();
            }

            ev->receive_param("_affectedEntity", affectedStr);

            std::string typeStr = ev->get_param("_type");

            if (typeStr.empty())
            {
                typeStr = "EXPLOSION";
            }
            base::Text("Particle Type");
            if (base::RadioButton("Explode", typeStr == "EXPLOSION"))
            {
                typeStr = "EXPLOSION";
            }
            if (base::RadioButton("Something Else", typeStr == "UNDEFINED"))
            {
                typeStr = "UNDEFINED";
            }
            ev->receive_param("_type", typeStr);

            return false;
        }
    };
}
