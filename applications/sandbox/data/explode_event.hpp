#pragma once
#include "animation_events.hpp"
#include <rendering/util/gui.hpp>
#include <core/core.hpp>
namespace ext::evt
{
    struct explosion_receiver { std::string name; };
    extern std::vector<legion::ecs::entity_handle> g_er_entities;

    struct explosion_event : public animation_event<explosion_event>
    {
        static bool onGUI(legion::id_type id, animation_event_base* ev) {
            using namespace imgui;
            base::Text("Explosion Event");
            std::string affectedStr = ev->get_param("_affectedEntity");

            base::Text("Affected Entity");
            base::SameLine();
            if (base::BeginCombo("##TypeEXEVT", affectedStr.data())) {

                for (const legion::ecs::entity_handle& handle : g_er_entities)
                {
                    auto er = handle.read_component<explosion_receiver>();
                    //we query the database for entries and display them in a combobox, which makes for a pretty useful chooser
                    if (ImGui::Selectable(std::string(er.name).c_str(), er.name == affectedStr))
                    {
                        affectedStr = er.name;
                    }
                }
                base::EndCombo();
            }

            ev->receive_param("_affectedEntity", affectedStr);

            std::string explosionStr = ev->get_param("_explosionStrength");

            base::Text("Explosion Strength");
            base::SameLine();
            if (base::BeginCombo("##TypeEXSTR", explosionStr.data())) {

                if (ImGui::Selectable("small explosion", explosionStr == "SMALL"))
                {
                    explosionStr = "SMALL";
                }
                if (ImGui::Selectable("medium explosion", explosionStr == "MEDIUM"))
                {
                    explosionStr = "MEDIUM";
                }
                if (ImGui::Selectable("big explosion", explosionStr == "BIG"))
                {
                    explosionStr = "BIG";
                }
                if (ImGui::Selectable("TZAR BOMBA", explosionStr == "AN602"))
                {
                    explosionStr = "AN602";
                }
                base::EndCombo();
            }


            ev->receive_param("_explosionStrength", explosionStr);

            return false;
        }

        std::string getAffectedName() const
        {
            return get_param("_affectedEntity");
        }
        std::string getExplosionStrength() const
        {
            return get_param("_explosionStrength");
        }
    };
}
