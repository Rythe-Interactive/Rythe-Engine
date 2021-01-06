#pragma once
#include <application/application.hpp>
#include <rendering/util/bindings.hpp>
#include <rendering/data/buffer.hpp>
#include <rendering/data/vertexarray.hpp>

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
        vertexarray vertexArray;
        buffer vertexBuffer;
        buffer colorBuffer;
        buffer normalBuffer;
        buffer uvBuffer;
        buffer tangentBuffer;
        buffer indexBuffer;

        std::vector<sub_mesh> submeshes;
    };

    /**@class model_handle
     * @brief Save to pass around handle to a model in the model cache.
     */
    struct model_handle
    {
        id_type id;
        
        bool operator==(const model_handle& other) const { return id == other.id; }
        bool is_buffered() const;
        void buffer_data(const buffer& matrixBuffer) const;
        mesh_handle get_mesh() const;
        const model& get_model() const;

        /*void serialize(cereal::JSONOutputArchive& archive);

        void serialize(cereal::JSONInputArchive& archive);*/
    };

    ////Out to disk, i.e Serialize
    //void model_handle::serialize(cereal::JSONOutputArchive& oarchive)
    //{
    //    size_type index = serialization::DataCache<model_handle>::append_list("ModelCache",*this);
    //    oarchive(id,cereal::make_nvp("ModelCache Index",index));
    //}

    ////In from disk, i.e Deserialize
    //void model_handle::serialize(cereal::JSONInputArchive& iarchive)
    //{
    //    size_type index;
    //    iarchive(cereal::make_nvp("ModelCache Index",index));
    //    model_handle modelH = serialization::DataCache<model_handle>::get_item_from_list("ModelCache", index);
    //    //ModelCache::create_model(modelH.get_mesh());
    //}

   

    constexpr model_handle invalid_model_handle { invalid_id };

    class ModelCache
    {
        friend class renderer;
        friend struct model_handle;
    private:
        static sparse_map<id_type, model> m_models;
        static async::rw_spinlock m_modelLock;

        static async::rw_spinlock m_modelNameLock;
        static std::unordered_map<id_type, std::string> m_modelNames;

        static const model& get_model(id_type id);

    public:
        static std::string get_model_name(id_type id);

        static void buffer_model(id_type id, const buffer& matrixBuffer);
        static model_handle create_model(const std::string& name, const fs::view& file, mesh_import_settings settings = default_mesh_settings);
        static model_handle create_model(const std::string& name);
        static model_handle create_model(const std::string& name, id_type meshId);
        static model_handle create_model(id_type meshId);
        static model_handle create_model(const std::string& name, mesh_handle mesh);
        static model_handle create_model(mesh_handle mesh);
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
