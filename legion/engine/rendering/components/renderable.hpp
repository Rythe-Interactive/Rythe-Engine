#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/material.hpp>

namespace legion::rendering
{
    struct mesh_renderer
    {
        mesh_renderer() = default;
        mesh_renderer(const material_handle& src) { material = src; }

        material_handle material;
    };


    struct renderable : public ecs::archetype<mesh_filter, mesh_renderer>
    {
        using base = ecs::archetype<mesh_filter, mesh_renderer>;

        renderable() = default;
        renderable(const base::handleGroup& handles) : base(handles) {}

        model_handle get_model()
        {
            id_type id = get<mesh_filter>().read().id;
            if (id == invalid_id)
                return { invalid_id };
            return ModelCache::create_model(id);
        }

        material_handle get_material()
        {
            return get<mesh_renderer>().read().material;
        }

        template<typename Archive>
        void serialize(Archive& archive);
    };

    template<typename Archive>
    void renderable::serialize(Archive& archive)
    {
        archive(get_model(), get_material());
    }
}
