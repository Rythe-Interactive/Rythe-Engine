#pragma once
#include <core/core.hpp>
#include <physics/mesh_splitter_utils/mesh_splitter.hpp>
#include <rendering/data/model.hpp>


namespace legion::rendering
{
    struct EmbeddedLoader
    {
        static void loader(ecs::entity_handle eH)
        {
            auto mInfo = eH.read_component<use_embedded_material>();

            static id_type acc = 1337;
            std::string name = "%%VERY_FAKE_YES%%" + std::to_string(acc++);

            std::vector<material_handle> mats;
            ModelCache::create_model(name,fs::view(mInfo.embedded_material_path),mats);
            if(!mats.empty()) eH.modify_component<mesh_renderer>([&mats](mesh_renderer& mr)
            {
                //TODO(glyn): have fun setting this automagically somehow
                mats[0].set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
                mr.material = mats[0];
            });
        }
    };
}
