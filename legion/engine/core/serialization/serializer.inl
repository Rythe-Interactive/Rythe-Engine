#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    inline bool serializer<ecs::entity_data>::serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view, std::string name)
    {
        s_view.start_object(name);
        s_view.serialize("\"id\"", ent_data.id);
        s_view.serialize("\"name\"", ent_data.name);

        s_view.serialize("\"alive\"", ent_data.alive);
        s_view.serialize("\"active\"", ent_data.active);

        if (ent_data.parent)
            s_view.serialize("\"parent\"", ent_data.parent->id);

        s_view.start_container("\"children\"");
        pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
        _serializer->serialize_container(ent_data.children, s_view);
        s_view.end_container();

        s_view.start_container("\" components\"");
        auto ent_composition = ecs::Registry::entityComposition(ent_data.id);

        for (id_type typeId : ent_composition)
        {
            s_view.start_object("");
            auto ent = ecs::Registry::getEntity(ent_data.id);
            auto compFamily = ecs::Registry::getFamily(typeId);
            auto _serializer = serializer_registry::get_serializer(typeId);
            auto comp = compFamily->create_component(ent);
            _serializer->serialize(comp, s_view, "\"" + ecs::Registry::getFamilyName(typeId) + "\"");
            s_view.end_object();
        }
        s_view.end_container();

        s_view.end_object();
        return true;
    }
    inline bool serializer<ecs::entity_data>::serialize(const std::any& ent, serializer_view& s_view, std::string name)
    {
        return serialize_ent_data(std::any_cast<ecs::entity_data>(ent), s_view, name);
    }

    inline bool serializer<ecs::entity>::serialize_ent(const ecs::entity& ent, serializer_view& s_view, std::string name)
    {
        s_view.start_object("");
        s_view.serialize("\"id\"", ent->id);
        s_view.serialize("\"name\"", ent->name);

        s_view.serialize("\"alive\"", ent->alive);
        s_view.serialize("\"active\"", ent->active);
        if (ent->parent)
            s_view.serialize("\"parent\"", ent->parent->id);

        s_view.start_container("\"children\"");
        pointer<serializer<ecs::entity_set>> _serializer = serializer_registry::get_serializer<ecs::entity_set>();
        _serializer->serialize_container(ent->children, s_view);
        s_view.end_container();

        s_view.start_container("\" components\"");
        auto ent_composition = ecs::Registry::entityComposition(ent->id);

        for (id_type typeId : ent_composition)
        {
            s_view.start_object("");
            auto compFamily = ecs::Registry::getFamily(typeId);
            auto _serializer = serializer_registry::get_serializer(typeId);
            auto comp = compFamily->create_component(ent);
            _serializer->serialize(comp, s_view, "\"" + ecs::Registry::getFamilyName(typeId) + "\"");
            s_view.end_object();
        }
        s_view.end_container();

        s_view.end_object();
        return true;
    }
    inline bool serializer<ecs::entity>::serialize(const std::any& ent, serializer_view& s_view, std::string name)
    {
        return serialize_ent(std::any_cast<ecs::entity>(ent), s_view, name);
    }

    template<typename type>
    inline bool serializer<type>::serialize_container(const std::any& container, serializer_view& s_view)
    {
        using container_type = typename remove_cvr_t<type>;
        auto _container = std::any_cast<container_type>(container);

        for (container_type::const_iterator it = _container.begin(); it != _container.end(); ++it)
        {
            using value_type = remove_cvr_t<decltype(*it._Ptr)>;
            pointer<serializer<value_type>> _serializer = serializer_registry::get_serializer<value_type>();
            _serializer->serialize(*it._Ptr, s_view, "");
        }

        return true;
    }

    template<typename type>
    inline bool serializer<type>::serialize(const std::any& serializable, serializer_view& s_view, std::string name)
    {
        using serializable_type = typename remove_cvr_t<type>;

        if constexpr (std::is_same<serializable_type, id_type>::value)
            s_view.serialize<unsigned long long int>(name, std::any_cast<unsigned long long int>(serializable));
        else if constexpr (is_container<type>::value)
        {
            s_view.start_container("\"Container\"");
            serialize_container(serializable, s_view);
            s_view.end_container();
        }
        else if constexpr (!std::is_constructible<serializable_type, const serializable_type&>::value)
        {
            s_view.start_object(name);
            pointer<serializer<serializable_type>> _serializer = serializer_registry::get_serializer<serializable_type>();
            _serializer->serialize(serializable, s_view, name);
            s_view.end_object();
        }
        else
        {
            auto _serializable = std::any_cast<serializable_type>(serializable);
            if (!s_view.serialize<serializable_type>(name, std::move(_serializable)))
            {
                auto reflector = make_reflector(_serializable);
                s_view.start_object(name);
                for_each(reflector,
                    [&s_view](auto& name, auto& value)
                    {
                        using value_type = typename remove_cvr_t<decltype(value)>;
                        auto _serializer = serializer_registry::get_serializer<value_type>();
                        _serializer->serialize(value, s_view, "\"" + name + "\"");
                    });
                s_view.end_object();
            }
        }
        return true;
    }

    template<typename type>
    inline bool serializer<type>::write(const std::any& serializable, std::string name, const fs::view& file)
    {
        auto jsonView = serialization::json_view();
        jsonView.data.append("{");
        auto result = serialize(serializable, jsonView, "\"" + name + "\"");
        jsonView.data.pop_back();
        jsonView.data.append("}");

        json j = json::parse(jsonView.data);

        std::ofstream of(fs::view_util::get_view_path(file, true));
        of << j.dump(4);
        of.close();

        return true;
    }

    inline prototype_base serializer<ecs::entity_data>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    inline prototype_base serializer<ecs::entity>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    template<typename serializable_type>
    inline prototype_base serializer<serializable_type>::deserialize(serializer_view& view)
    {
        return prototype_base();
    }

    inline bool read(const fs::view& view)
    {
        return false;
    }
}
