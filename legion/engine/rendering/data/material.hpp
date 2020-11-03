#pragma once
#include <rendering/data/shader.hpp>

#include <memory>

namespace legion::rendering
{
    struct material;

    struct material_parameter_base
    {
    protected:
        std::string m_name;
        id_type m_id;
        id_type m_typeId;
        GLint m_location;

        material_parameter_base(const std::string& name, GLint location, id_type typeId) : m_name(name), m_id(nameHash(name)), m_typeId(typeId), m_location(location) {}

    public:
        id_type type() { return m_typeId; }

        virtual void apply(shader_handle& shader) LEGION_PURE;

        static material_parameter_base* create_param(const std::string& name, const GLint& location, const GLenum& type);
    };

    template<typename T>
    struct material_parameter : public material_parameter_base
    {
    private:
        T m_value;
    public:
        material_parameter(const std::string& name, GLint location) : material_parameter_base(name, location, typeHash<T>()) {}

        void set_value(const T& value) { m_value = value; }
        T get_value() const { return m_value; }

        virtual void apply(shader_handle& shader) override
        {
            shader.get_uniform<T>(m_id).set_value(m_value);
        }
    };

    struct material
    {
        friend class MaterialCache;
        friend struct material_handle;
    private:
        shader_handle m_shader;

        void init(const shader_handle& shader)
        {
            m_shader = shader;
            for (auto& [name, location, type] : m_shader.get_uniform_info())
            {
                id_type hash = nameHash(name);
                m_parameters.emplace(std::make_pair(hash, material_parameter_base::create_param(name, location, type)));
                m_idOfLocation[location] = hash;
            }
        }

        std::string m_name;

        std::unordered_map<id_type, std::unique_ptr<material_parameter_base>> m_parameters;
        std::unordered_map<GLint, id_type> m_idOfLocation;
    public:
        template<typename T>
        void set_param(const std::string& name, const T& value)
        {
            id_type id = nameHash(name);
            if (m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>())
                static_cast<material_parameter<T>*>(m_parameters[id].get())->set_value(value);
            else
                log::error("material {} does not have a parameter named {} of type {}", m_name, name, undecoratedTypeName<T>());
        }

        template<typename T>
        bool has_param(const std::string& name)
        {
            id_type id = nameHash(name);
            return m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>();
        }

        template<typename T>
        T get_param(const std::string& name)
        {
            id_type id = nameHash(name);
            if (m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>())
                return static_cast<material_parameter<T>*>(m_parameters[id].get())->get_value();

            log::error("material {} does not have a parameter named {} of type {}", m_name, name, undecoratedTypeName<T>());
            return T();
        }

        template<typename T>
        void set_param(GLint location, const T& value)
        {
            if(!m_idOfLocation.count(location))
                log::error("material {} does not have a parameter at location {} of type {}", m_name, location, undecoratedTypeName<T>());

            id_type id = m_idOfLocation[location];

            if (m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>())
                static_cast<material_parameter<T>*>(m_parameters[id].get())->set_value(value);
            else
                log::error("material {} does not have a parameter at location {} of type {}", m_name, location, undecoratedTypeName<T>());
        }

        template<typename T>
        bool has_param(GLint location)
        {
            if (!m_idOfLocation.count(location))
                return false;

            id_type id = m_idOfLocation[location];
            return m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>();
        }

        template<typename T>
        T get_param(GLint location)
        {
            if (!m_idOfLocation.count(location))
                log::error("material {} does not have a parameter at location {} of type {}", m_name, location, undecoratedTypeName<T>());

            id_type id = m_idOfLocation[location];
            if (m_parameters.count(id) && m_parameters[id]->type() == typeHash<T>())
                return static_cast<material_parameter<T>*>(m_parameters[id].get())->get_value();

            log::error("material {} does not have a parameter at location {} of type {}", m_name, name, undecoratedTypeName<T>());
            return T();
        }

        attribute get_attribute(const std::string& name)
        {
            return m_shader.get_attribute(nameHash(name));
        }
    };

    struct material_handle
    {
        id_type id;

        void bind();

        void prepare();

        void release()
        {
            shader_handle::release();
        }

        template<typename T>
        void set_param(const std::string& name, const T& value);
        template<typename T>
        void set_param(GLint location, const T& value);

        template<typename T>
        bool has_param(const std::string& name);
        template<typename T>
        bool has_param(GLint location);

        template<typename T>
        T get_param(const std::string& name);
        template<typename T>
        T get_param(GLint location);

        attribute get_attribute(const std::string& name);

        bool operator==(const material_handle& other) const { return id == other.id; }
    };

    constexpr material_handle invalid_material_handle{ invalid_id };

    class MaterialCache
    {
        friend struct material_handle;
    private:
        static async::readonly_rw_spinlock m_materialLock;
        static std::unordered_map<id_type, material> m_materials;

    public:
        static material_handle create_material(const std::string& name, const shader_handle& shader);
        static material_handle create_material(const std::string& name, const filesystem::view& shaderFile, shader_import_settings settings = default_shader_settings);
        static material_handle get_material(const std::string& name);
    };

    template<typename T>
    void material_handle::set_param(const std::string& name, const T& value)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        MaterialCache::m_materials[id].set_param<T>(name, value);
    }

    template<typename T>
    void material_handle::set_param(GLint location, const T& value)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        MaterialCache::m_materials[id].set_param<T>(location, value);
    }

    template<typename T>
    inline bool material_handle::has_param(const std::string& name)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        return MaterialCache::m_materials[id].has_param<T>(name);
    }

    template<typename T>
    inline bool material_handle::has_param(GLint location)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        return MaterialCache::m_materials[id].has_param<T>(location);
    }

    template<typename T>
    T material_handle::get_param(const std::string& name)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        return MaterialCache::m_materials[id].get_param<T>(name);
    }

    template<typename T>
    T material_handle::get_param(GLint location)
    {
        async::readonly_guard guard(MaterialCache::m_materialLock);
        return MaterialCache::m_materials[id].get_param<T>(location);
    }

}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::rendering::material_handle>
    {
        std::size_t operator()(legion::rendering::material_handle const& handle) const noexcept
        {
            return static_cast<std::size_t>(handle.id);
        }
    };
}
#endif
