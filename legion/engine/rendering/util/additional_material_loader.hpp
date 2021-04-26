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
            static const auto defaultLitShader = ShaderCache::create_shader("default lit", fs::view("engine://shaders/default_lit.shs"));

            auto mInfo = eH.read_component<use_embedded_material>();

            static const std::string name = "%%VERY_FAKE_YES%%";

            material_handle material = MaterialCache::create_material(mInfo.materialName, defaultLitShader);

            material_list loadedMaterials;
            auto settings = default_mesh_settings;
            settings.materials = &loadedMaterials;
            ModelCache::create_model(name, fs::view(mInfo.embedded_material_path), settings);

            if (loadedMaterials.size())
            {
                auto& mat = loadedMaterials[0];

                if (mat.doubleSided)
                    material.set_variant("double_sided");

                material.set_param("alphaCutoff", mat.alphaCutoff);

                if (mat.albedoMap)
                {
                    material.set_param("useAlbedoTex", true);
                    material.set_param("albedoTex", TextureCache::create_texture_from_image(mat.albedoMap));
                }
                else
                {
                    material.set_param("useAlbedoTex", false);
                    material.set_param("albedoColor", mat.albedoValue);
                }

                if (mat.metallicRoughnessMap)
                {
                    material.set_param("useMetallicRoughness", true);
                    material.set_param("metallicRoughness", TextureCache::create_texture_from_image(mat.metallicRoughnessMap));
                }
                else
                {
                    material.set_param("useMetallicRoughness", false);

                    if (mat.metallicMap)
                    {
                        material.set_param("useMetallicTex", true);
                        material.set_param("metallicTex", TextureCache::create_texture_from_image(mat.metallicMap));
                    }
                    else
                    {
                        material.set_param("useMetallicTex", false);
                        material.set_param("metallicValue", mat.metallicValue);
                    }

                    if (mat.roughnessMap)
                    {
                        material.set_param("useRoughnessTex", true);
                        material.set_param("roughnessTex", TextureCache::create_texture_from_image(mat.roughnessMap));
                    }
                    else
                    {
                        material.set_param("useRoughnessTex", false);
                        material.set_param("roughnessValue", mat.roughnessValue);
                    }
                }

                if (mat.emissiveMap)
                {
                    material.set_param("useEmissiveTex", true);
                    material.set_param("emissiveTex", TextureCache::create_texture_from_image(mat.emissiveMap));
                }
                else
                {
                    material.set_param("useEmissiveTex", false);
                    material.set_param("emissiveColor", mat.emissiveValue);
                }

                if (mat.normalMap)
                {
                    material.set_param("useNormal", true);
                    material.set_param("normalTex", TextureCache::create_texture_from_image(mat.normalMap));
                }
                else
                {
                    material.set_param("useNormal", false);
                }

                if (mat.aoMap)
                {
                    material.set_param("useAmbientOcclusion", true);
                    material.set_param("ambientOcclusionTex", TextureCache::create_texture_from_image(mat.aoMap));
                }
                else
                {
                    material.set_param("useAmbientOcclusion", false);
                }

                if (mat.heightMap)
                {
                    material.set_param("useHeight", true);
                    material.set_param("heightTex", TextureCache::create_texture_from_image(mat.heightMap));
                }
                else
                {
                    material.set_param("useHeight", false);
                }

                material.setLoadOrSaveBit(false);
                log::debug("Loaded embedded material {}", material.get_name());
            }

            ModelCache::destroy_model(name);

            eH.modify_component<mesh_renderer>([&material](mesh_renderer& mr) {mr.material = material; });
        }
    };
}
