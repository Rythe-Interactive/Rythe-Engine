#pragma once
#include <application/application.hpp>
#include <rendering/util/bindings.hpp>
#include <vector>
#include <unordered_map>
#include <set>

/**
 * @file model.hpp
 */

namespace legion::rendering
{
    /**@class model
     * @brief Struct containing all the buffer ids and the VAO needed for rendering.
     */
    struct model
    {
        bool buffered;
        app::gl_id vertexArrayId;
        app::gl_id vertexBufferId;
        app::gl_id colorBufferId;
        app::gl_id normalBufferId;
        app::gl_id uvBufferId;
        app::gl_id tangentBufferId;
        app::gl_id indexBufferId;

        std::vector<sub_mesh> submeshes;
    };

    /**@class model_handle
     * @brief Save to pass around handle to a model in the model cache.
     */
    struct model_handle
    {
        id_type id;
        
        bool operator==(const model_handle& other) const { return id == other.id; }
        bool is_buffered();
        void buffer_data(app::gl_id matrixBuffer);
        mesh_handle get_mesh();
        const model& get_model();

        template<typename Archive>
        void serialize(Archive& archive);
    };

    template<typename Archive>
    void model_handle::serialize(Archive& archive)
    {
        archive(id);
    }

    constexpr model_handle invalid_model_handle { invalid_id };

    class ModelCache
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

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::rendering::model_handle>
    {
        std::size_t operator()(legion::rendering::model_handle const& handle) const noexcept
        {
            return static_cast<std::size_t>(handle.id);
        }
    };
}
#endif
