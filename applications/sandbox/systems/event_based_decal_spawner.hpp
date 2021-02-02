#pragma once
#include <core/core.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/data/material.hpp>
#include <rendering/data/model.hpp>
#include <rendering/data/texture.hpp>
#include <rendering/util/bindings.hpp>


#include "../data/explode_event.hpp"

namespace ext
{

    using namespace legion;
    class DecalSpawner : public System<DecalSpawner>
    {
        rendering::model_handle cubeModel;
        rendering::material_handle vertexColorMaterial;

        void setup() override
        {
            bindToEvent<evt::explosion_event, &DecalSpawner::onSpawnDecal>();
            app::window window = m_ecs->world.get_component_handle<app::window>().read();
            {
                application::context_guard guard(window);

                cubeModel = rendering::ModelCache::create_model("cube", fs::view("assets://models/cube.obj"));
                vertexColorMaterial = rendering::MaterialCache::create_material("decal", fs::view("assets://shaders/decal.shs"));
                vertexColorMaterial.set_param(SV_ALBEDO, rendering::TextureCache::create_texture(fs::view("engine://resources/default/albedo")));
                vertexColorMaterial.set_param(SV_NORMALHEIGHT, rendering::TextureCache::create_texture(fs::view("engine://resources/default/normalHeight")));
                vertexColorMaterial.set_param(SV_MRDAO, rendering::TextureCache::create_texture(fs::view("engine://resources/default/MRDAo")));
                vertexColorMaterial.set_param(SV_EMISSIVE, rendering::TextureCache::create_texture(fs::view("engine://resources/default/emissive")));
                vertexColorMaterial.set_param(SV_HEIGHTSCALE, 0.f);
                vertexColorMaterial.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
            }

        }

        void onSpawnDecal(evt::explosion_event* ev)
        {
            static auto query = createQuery<evt::explosion_receiver>();

            ecs::entity_handle handle;
            query.queryEntities();
            for (ecs::entity_handle h : query)
            {
                if (ev->getAffectedName() == h.read_component<evt::explosion_receiver>().name)
                {
                    handle = h;
                }
            }

            const auto [pos,rot,_] = handle.get_component_handles<transform>();
            auto ent = createEntity();

            ent.add_components<transform>(pos.read(),rot.read(),scale(2));
            ent.add_components<rendering::mesh_renderable>(mesh_filter(cubeModel.get_mesh()), rendering::mesh_renderer(vertexColorMaterial));


        }
    };
}
