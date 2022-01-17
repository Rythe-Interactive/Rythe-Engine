#include <core/serialization/serializer.hpp>
#pragma once

#define EndReadPropagate(_result, _warnings, _view)                                                 \
        _warnings.insert(_warnings.end(), _result.warnings().begin(), _result.warnings().end());    \
        if (_result.has_error()) { _view.end_read(); return { _result.error(), _warnings }; }

#define EndObjectPropagate(_result, _warnings, _view)                                               \
        _warnings.insert(_warnings.end(), _result.warnings().begin(), _result.warnings().end());    \
        if (_result.has_error()) { _view.end_object(); return { _result.error(), _warnings }; }

namespace legion::core::serialization
{
    namespace detail
    {
        template<typename type>
        inline common::result<void, fs_error> serialize_container(type&& container, serializer_view& s_view, std::string_view name)
        {
            using container_type = remove_cvr_t<type>;
            using value_type = remove_cvr_t<typename container_type::value_type>;

            s_view.start_container(std::string(name));

            std::vector<std::string> warnings{};
            size_type i = 0;
            for (auto it = container.begin(); it != container.end(); ++it, i++)
            {
                if constexpr (is_serializable_v<value_type>)
                {
                    if (!s_view.serialize("", *it))
                    {
                        s_view.end_container();
                        return { legion_fs_error("Type was not a primitive serializable type."), warnings };
                    }
                }
                else if constexpr (is_container_v<value_type>)
                {
                    auto result = detail::serialize_container(*it, s_view, "");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_container();
                        return { result.error(), warnings };
                    }
                }
                else
                {
                    auto _serializer = SerializerRegistry::getSerializer<value_type>();

                    auto result = _serializer->serialize(&(*it), s_view, "");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_container();
                        return { result.error(), warnings };
                    }
                }
            }

            s_view.end_container();

            return { common::success, warnings };
        }

        template<typename type>
        inline common::result<void, fs_error> deserialize_container(type& container, serializer_view& s_view, std::string_view name)
        {
            using container_type = remove_cvr_t<type>;
            using value_type = remove_cvr_t<typename container_type::value_type>;
            std::vector<std::string> warnings{};

            {
                auto result = s_view.start_read(std::string(name));
                PropagateErrors(result, warnings);
            }

            size_type size = s_view.current_item_size();

            std::vector<value_type> tempContainer{};
            tempContainer.reserve(size);

            byte buffer[sizeof(value_type)];
            value_type* itemPtr = reinterpret_cast<value_type*>(buffer);

            for (size_type i = 0; i < size; i++)
            {
                if constexpr (is_serializable_v<value_type>)
                {
                    auto result = s_view.deserialize<value_type>("");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_read();
                        return { result.error(), warnings };
                    }
                    new(itemPtr) value_type(*result);
                }
                else if constexpr (is_container_v<value_type>)
                {
                    auto result = detail::deserialize_container(*itemPtr, s_view, "");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_read();
                        return { result.error(), warnings };
                    }
                }
                else
                {
                    auto _serializer = SerializerRegistry::getSerializer<value_type>();

                    auto result = _serializer->deserialize(buffer, s_view, "");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_read();
                        return { result.error(), warnings };
                    }
                }

                tempContainer.push_back(*itemPtr);
            }

            s_view.end_read();

            using iterator = typename container_type::iterator;
            using const_iterator = typename container_type::const_iterator;
            using input_iterator = typename std::vector<value_type>::iterator;

            if constexpr (has_insert_v<container_type, iterator(const_iterator, input_iterator, input_iterator)>)
                container.insert(container.begin(), tempContainer.begin(), tempContainer.end());
            else if constexpr (has_insert_v<container_type, void(input_iterator, input_iterator)>)
                container.insert(tempContainer.begin(), tempContainer.end());
            else if constexpr (has_size_v<container_type, size_type()> && has_at_v<container_type, value_type & (size_type)>)
            {
                for (size_type i = 0; i < tempContainer.size(); i++)
                    container[i] = tempContainer[i];
            }

            return { common::success, warnings };
        }

        inline common::result<void, fs_error> serialize_ent_data(const ecs::entity_data& ent_data, serializer_view& s_view, std::string_view name)
        {
            if (!ent_data.alive)
                return legion_fs_error("Entity does not exist.");

            std::vector<std::string> warnings{};

            s_view.start_object(std::string(name));
            s_view.serialize<std::string>("type_name", nameOfType<ecs::entity_data>());
            s_view.serialize("name", ent_data.name);
            s_view.serialize("active", ent_data.active);
            std::vector<ecs::entity> children{ ent_data.children.begin(), ent_data.children.end() };
            auto result = detail::serialize_container(children, s_view, "children");
            EndObjectPropagate(result, warnings, s_view);

            s_view.start_container("components");

            auto ent = ecs::Registry::getEntity(ent_data.id);
            auto ent_composition = ent.component_composition();

            for (id_type typeId : ent_composition)
            {
                auto compName = std::string(type_hash::from_id(typeId).name());
                if (compName.empty())
                {
                    s_view.end_container();
                    s_view.end_object();
                    return { legion_fs_error("Component type " + std::to_string(typeId) + " has no type data."), warnings };
                }

                auto _serializer = SerializerRegistry::getSerializer(typeId);
                if (!_serializer)
                {
                    warnings.push_back("Could not find existing serializer for " + compName);
                    continue;
                }

                auto result = _serializer->serialize(ecs::Registry::getComponent(typeId, ent), s_view, compName);
                warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                if (result.has_error())
                {
                    s_view.end_container();
                    s_view.end_object();
                    return { result.error(), warnings };
                }
            }

            s_view.end_container();
            s_view.end_object();
            return { common::success, warnings };
        }

        inline common::result<void, fs_error> deserialize_ent_data(ecs::entity_data* target, serializer_view& s_view, std::string_view name)
        {
            std::vector<std::string> warnings{};

            {
                auto result = s_view.start_read(std::string(name)); // Object
                PropagateErrors(result, warnings);
            }

            {
                auto result = s_view.deserialize<std::string>("type_name");
                EndReadPropagate(result, warnings, s_view);

                if (*result != nameOfType<ecs::entity_data>())
                {
                    s_view.end_read();
                    return { legion_fs_error("Item of name " + std::string(name) + " is not of type " + std::string(nameOfType<ecs::entity_data>()) + " but of type " + *result + "."), warnings };
                }
            }

            // Entity
            auto ent = ecs::Registry::createEntity();

            {
                auto result = s_view.deserialize<std::string>("name");
                EndReadPropagate(result, warnings, s_view);
                ent->name = *result;
            }

            {
                auto result = s_view.deserialize<bool>("active");
                EndReadPropagate(result, warnings, s_view);
                ent->active = *result;
            }

            // Children
            std::vector<ecs::entity> children{};
            {
                auto result = deserialize_container(children, s_view, "children");
                EndReadPropagate(result, warnings, s_view);
            }

            for (auto& child : children)
                ent.add_child(child);

            // Components
            {
                auto result = s_view.start_read("components");
                EndReadPropagate(result, warnings, s_view);
            }

            for (size_type i = 0; i < s_view.current_item_size(); i++)
            {
                s_view.peak_ahead("");
                id_type typeId;

                {
                    auto result = s_view.deserialize<std::string>("type_name");
                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_read(); // Components
                        s_view.end_read(); // Object
                        return { result.error(), warnings };
                    }

                    typeId = nameHash(*result);
                }

                auto _serializer = SerializerRegistry::getSerializer(typeId);

                byte_vec buffer{};
                buffer.resize(_serializer->type_size());

                {
                    auto result = _serializer->deserialize(buffer.data(), s_view, "");

                    warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());

                    if (result.has_error())
                    {
                        s_view.end_read(); // Components
                        s_view.end_read(); // Object
                        return { result.error(), warnings };
                    }
                }

                ecs::Registry::createComponent(typeId, ent, buffer.data());
            }

            s_view.end_read(); // Components
            s_view.end_read(); // Object

            new(target) ecs::entity_data(*ent.data);

            return { common::success, warnings };
        }
    }

    inline common::result<void, fs_error> serializer<ecs::entity_data>::serialize(const void* ent, serializer_view& s_view, std::string_view name)
    {
        return detail::serialize_ent_data(*static_cast<const ecs::entity_data*>(ent), s_view, name);
    }

    inline common::result<void, fs_error> serializer<ecs::entity_data>::deserialize(void* target, serializer_view& view, std::string_view name)
    {
        return detail::deserialize_ent_data(reinterpret_cast<ecs::entity_data*>(target), view, name);
    }

    inline common::result<void, fs_error> serializer<ecs::entity>::serialize(const void* ent, serializer_view& s_view, std::string_view name)
    {
        return detail::serialize_ent_data(*static_cast<const ecs::entity*>(ent)->data, s_view, name);
    }

    inline common::result<void, fs_error> serializer<ecs::entity>::deserialize(void* target, serializer_view& view, std::string_view name)
    {
        byte rawData[sizeof(ecs::entity_data)];
        ecs::entity_data* data = reinterpret_cast<ecs::entity_data*>(rawData);
        auto result = detail::deserialize_ent_data(data, view, name);
        *static_cast<ecs::entity*>(target) = ecs::Registry::getEntity(data->id);
        return result;
    }

    template<typename type>
    inline common::result<void, fs_error> serializer<type>::serialize(const void* serializable, serializer_view& s_view, std::string_view name)
    {
        using serializable_type = remove_cvr_t<type>;

        auto* ptr = static_cast<const serializable_type*>(serializable);

        if constexpr (is_serializable_v<serializable_type>)
        {
            return s_view.serialize(std::string(name), *ptr) ? common::success : legion_fs_error("Type was not a primitive serializable type.");
        }
        else if constexpr (is_container_v<serializable_type>)
        {
            return detail::serialize_container(*ptr, s_view, name);
        }
        else
        {
            auto refl = make_reflector(*ptr);
            std::vector<std::string> warnings{};

            s_view.start_object(std::string(name));
            s_view.serialize<std::string>("type_name", nameOfType<serializable_type>());

            for (auto& var : refl.members)
            {
                if (var.is_object)
                {
                    auto _serializer = SerializerRegistry::getSerializer(var.object.typeId);
                    auto result = _serializer->serialize(var.object.data, s_view, var.name);
                    EndObjectPropagate(result, warnings, s_view);
                }
                else
                {
                    if (!s_view.serialize(std::string(var.name), var.primitive.data, var.primitive.typeId))
                    {
                        s_view.end_object();
                        return { legion_fs_error("Type was not a primitive serializable type."), warnings };
                    }
                }
            }

            s_view.end_object();
            return { common::success, warnings };
        }
    }

    template<typename type>
    inline common::result<void, fs_error> serializer<type>::deserialize(void* target, serializer_view& s_view, std::string_view name)
    {
        using serializable_type = remove_cvr_t<type>;
        auto* ptr = static_cast<serializable_type*>(target);

        if constexpr (is_serializable_v<serializable_type>)
        {
            auto result = s_view.deserialize<serializable_type>(std::string(name));
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
                new(ptr) serializable_type(result.value());
        }
        else  if constexpr (is_container_v<serializable_type>)
        {
            new(ptr) serializable_type();
            return detail::deserialize_container(*ptr, s_view, name);
        }
        else
        {
            new(ptr) serializable_type();
            reflector refl = make_reflector(*ptr);
            std::vector<std::string> warnings{};

            {
                auto result = s_view.start_read(std::string(name));
                PropagateErrors(result, warnings);
            }

            {
                auto result = s_view.deserialize<std::string>("type_name");
                EndReadPropagate(result, warnings, s_view);

                if (*result != nameOfType<serializable_type>())
                {
                    s_view.end_read();
                    return { legion_fs_error("Item of name " + std::string(name) + " is not of type " + std::string(nameOfType<serializable_type>()) + " but of type " + *result + "."), warnings };
                }
            }

            for (auto& var : refl.members)
            {
                if (var.is_object)
                {
                    auto _serializer = SerializerRegistry::getSerializer(var.object.typeId);
                    auto result = _serializer->deserialize(var.object.data, s_view, var.name);
                    EndReadPropagate(result, warnings, s_view);
                }
                else
                {
                    auto result = s_view.deserialize(std::string(var.name), var.primitive.data, var.primitive.typeId);
                    EndReadPropagate(result, warnings, s_view);
                }
            }
            s_view.end_read();

            return { common::success, warnings };
        }
    }
}

#undef EndReadPropagate
#undef EndObjectPropagate
