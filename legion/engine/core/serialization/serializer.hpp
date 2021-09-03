#pragma once
#include <core/filesystem/filesystem.hpp>
#include <core/serialization/serializer_view.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>
#include <nlohmann/json.hpp>

#include <sstream>
#include <fstream>

namespace legion::core::serialization
{
    using json = nlohmann::ordered_json;

    enum DataFormat
    {
        JSON,
        BSON,
        YAML
    };

    struct serializer_base
    {
        serializer_base() = default;
        ~serializer_base() = default;

        virtual common::result<std::any> serialize(const std::any& serializable, serializer_view& view) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };


    template<typename serializable_type>
    struct serializer : serializer_base
    {
        virtual common::result<serializable_type> serialize(const std::any& serializable, serializer_view& view) override
        {
            if (!serializable.has_value())
                return legion_exception_msg("invalid input data: serializable is null");

            if (serializable.type() != typeid(serializable_type))
                return legion_exception_msg("invalid input data: serializable is not of type %s, instead of type %s",typeid(serializable_type),typeid(serializable.type()));

            if (!view.serialize<serializable_type>(std::any_cast<serializable_type>(serializable)))
            {
                auto reflector = make_reflector(serializable);

                for_each(reflector,
                    [&j](auto& name, auto& value)
                    {
                        if constexpr(!std::is_same<decltype(value),serializable_type>::value)
                            serializer_registry::get_serializer<decltype(value)>().serialize(value, view);
                    });
            }
        };

        virtual prototype<serializable_type> deserialize(const std::string data, serializer_view& view) override
        {
            //if(format == DataFormat::BSON)
            //	return this->deserialize_impl<bson_view<serializable_type>>(data);
            //else if(format == DataFormat::YAML)
            //	return this->deserialize_impl<yaml_view<serializable_type>>(data);
            //else
            //	return this->deserialize_impl<json_view<serializable_type>>(data);
        }

    };
}




