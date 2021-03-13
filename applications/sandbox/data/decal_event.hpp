#pragma once
#include "animation_events.hpp"
#include <rendering/util/gui.hpp>

namespace ext::evt
{
    struct decal_receiver { std::string name; };
    extern std::vector<legion::ecs::entity_handle> g_dr_entities;

    struct decal_event : animation_event<decal_event>
    {
        static bool onGUI(legion::id_type id, animation_event_base* ev) {
            using namespace imgui;
            base::Text("Decal Event");

            std::string affectedStr = ev->get_param("_affectedEntity");

            base::Text("Affected Entity");
            base::SameLine();
            if (base::BeginCombo("##TypeDEEVT", affectedStr.data())) {

                for (const legion::ecs::entity_handle& handle : g_dr_entities)
                {
                    auto er = handle.read_component<decal_receiver>();
                    //we query the database for entries and display them in a combobox, which makes for a pretty useful chooser
                    if (ImGui::Selectable(std::string(er.name).c_str(), er.name == affectedStr))
                    {
                        affectedStr = er.name;
                    }
                }
                base::EndCombo();
            }

            ev->receive_param("_affectedEntity", affectedStr);

            return false;
        }
    };
}
