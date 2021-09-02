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

        virtual void serialize(const std::any& serializable,serializer_view& view) = 0;
        virtual prototype_base deserialize(serializer_view& view) = 0;
    };


    template<typename serializable_type>
    struct serializer : serializer_base
    {
        virtual void serialize(const std::any& serializable, serializer_view& view) override
        {
            if (!serializable.has_value())
                return;

            if (serializable.type != typeid(serializable_type))
                return;

            if (!view.serialize<serializable_type>(std::any_cast<serializable_type>(serializable)))
            {
                // get reflector;
                // iterate reflector;
                // call serializer of reflected type.
            }
        };

        virtual prototype_base deserialize(const std::string data, serializer_view& view) override
        {
            //if(format == DataFormat::BSON)
            //	return this->deserialize_impl<bson_view<serializable_type>>(data);
            //else if(format == DataFormat::YAML)
            //	return this->deserialize_impl<yaml_view<serializable_type>>(data);
            //else
            //	return this->deserialize_impl<json_view<serializable_type>>(data);
        }

    private:

        template<typename view_type>
        std::string serialize_impl(const serializable_type& serializable)
        {
            if constexpr (std::is_same<serializable_type, int>)
            {
                view_type::serialize_int(serializable);
            }
            else if constexpr (std::is_same<serializable_type, float>)
            {
                view_type::serialize_float(serializable);
            }
            else if constexpr (std::is_same<serializable_type, double>)
            {
                view_type::serialize_double(serializable);
            }
            else if constexpr (std::is_same<serializable_type, bool>)
            {
                view_type::serialize_bool(serializable);
            }
            else if constexpr (std::is_same<serializable_type, std::string>)
            {
                view_type::serialize_string(serializable);
            }
            else if constexpr (std::is_same<serializable_type, id_type>)
            {
                view_type::serialize_id_type(serializable);
            }
            else
            {
                const prototype<serializable_type>& prot(serializable);
            }

            return "";
        }

        template<typename view_type>
        prototype<serializable_type> deserialize_impl(const std::string data)
        {
            return view_type::deserialize(data);
        }
    };
}

////Serializer should get the type of the thing we are serializing
//template<typename serializable_type>
//struct serializer : public serializer_base
//{
//    //returns the serialized json
//    json serialize(const serializable_type& data, const SerializeFormat format);
//
//    json serialize_prototype(const serializable_type data, const SerializeFormat format);
//
//    //return prototype of type
//    prototype<serializable_type> deserialize(const json j, const SerializeFormat format);
//
//    //writes the given data to a file specified
//    void write(const fs::view filePath, const serializable_type& data, const SerializeFormat format);
//    //reads the data from the file specified
//    serializable_type read(const fs::view filePath, const SerializeFormat format);
//
//    using value_type = serializable_type;
//};



