#pragma once
#include <core/filesystem/assetimporter.hpp>
#include <core/data/mesh.hpp>

namespace args::core
{
    struct ARGS_API obj_mesh_loader : public filesystem::resource_converter<mesh, mesh_import_settings>
    {
        virtual common::result_decay_more<filesystem::basic_resource, fs_error> load(const filesystem::basic_resource& resource, mesh_import_settings&& settings) override;
    };
}
