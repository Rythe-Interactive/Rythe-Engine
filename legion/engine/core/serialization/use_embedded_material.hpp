#pragma once
#include <string>
#include <cereal/cereal.hpp>

namespace legion::core
{
    struct use_embedded_material
    {
        use_embedded_material(const std::string& matName, const std::string& str) : embedded_material_path(str), materialName(matName) {}
        use_embedded_material() = default;
        use_embedded_material(const use_embedded_material& other) = default;
        use_embedded_material(use_embedded_material&& other) noexcept = default;
        use_embedded_material& operator=(const use_embedded_material& other) = default;
        use_embedded_material& operator=(use_embedded_material&& other) noexcept = default;
        std::string embedded_material_path;
        std::string materialName;

        template <class Archive>
        void serialize(Archive& oa)
        {
            oa(cereal::make_nvp("EmbeddedMaterialPath",embedded_material_path), cereal::make_nvp("MaterialName", materialName));
        }
    };
}
