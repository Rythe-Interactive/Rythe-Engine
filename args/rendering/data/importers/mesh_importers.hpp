#pragma once
#include <rendering/data/model.hpp>

namespace args::rendering
{
    struct ARGS_API obj_mesh_loader : public fs::resource_converter<mesh, mesh_import_settings>
    {
        virtual common::result_decay_more<fs::basic_resource, fs_error> load(const fs::basic_resource& resource, mesh_import_settings&& settings) override;
    };
}
