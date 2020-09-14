#pragma once
#include <rendering/detail/engine_include.hpp>

#include <vector>
#include <unordered_map>
#include <set>

#define SV_START 8
#define SV_POSITION SV_START
#define SV_NORMAL SV_START + 1
#define SV_TANGENT SV_START + 2
#define SV_TEXCOORD0 SV_START + 3
#define SV_MODELMATRIX SV_START + 4
#define SV_VIEW SV_START
#define SV_PROJECT SV_START + 4

namespace args::rendering
{
    struct model
    {
        bool buffered;
        app::gl_id vertexArrayId;
        app::gl_id vertexBufferId;
        app::gl_id normalBufferId;
        app::gl_id uvBufferId;
        app::gl_id tangentBufferId;
        app::gl_id indexBufferId;

        std::vector<sub_mesh> submeshes;
    };

    struct ARGS_API model_handle
    {
        id_type id;
        
        bool operator==(const model_handle& other) const { return id == other.id; }
        bool is_buffered();
        void buffer_data(app::gl_id matrixBuffer);
        mesh_handle get_mesh();
        const model& get_model();
    };

    constexpr model_handle invalid_model_handle { invalid_id };

    class ARGS_API ModelCache
    {
        friend class renderer;
        friend struct model_handle;
    private:
        static sparse_map<id_type, model> m_models;
        static async::readonly_rw_spinlock m_modelLock;

        static const model& get_model(id_type id);

    public:
        static void buffer(id_type id, app::gl_id matrixBuffer);
        static model_handle create_model(const std::string& name, const fs::view& file, mesh_import_settings settings = default_mesh_settings);
        static model_handle get_handle(const std::string& name);
        static model_handle get_handle(id_type id);
        static mesh_handle get_mesh(const std::string& name);
        static mesh_handle get_mesh(id_type id);

    };
}

namespace std
{
    template<>
    struct hash<args::rendering::model_handle>
    {
        std::size_t operator()(args::rendering::model_handle const& handle) const noexcept
        {
            return static_cast<std::size_t>(handle.id);
        }
    };
}
