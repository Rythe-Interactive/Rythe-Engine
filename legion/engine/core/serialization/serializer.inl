#include <core/serialization/serializer.hpp>
#include <core/serialization/jsonview/json_view.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename serializable_type>
    inline json serializer<serializable_type>::serialize(const serializable_type &data, const SerializeFormat format)
    {
        json j;
        switch (format)
        {
        case JSON:
            return json_view<prototype<serializable_type>>().serialize(prototype<serializable_type>(data));
        case BINARY:
            return j;
        case YAML:
            return j;
        }
        return j;
    }

    template<typename serializable_type>
    inline json serializer<serializable_type>::serialize_prototype(const serializable_type data, const SerializeFormat format)
    {
        json j;
        switch (format)
        {
        case JSON:
            return json_view<serializable_type>().serialize(serializable_type(data));
        case BINARY:
            return j;
        case YAML:
            return j;
        }
        return j;
    }

    template<typename serializable_type>
    inline prototype<serializable_type> serializer<serializable_type>::deserialize(const json j, const SerializeFormat format)
    {
        switch (format)
        {
        case JSON:
            return json_view<prototype<serializable_type>>().deserialize(j);
        case BINARY:
            break;
        case YAML:
            break;
        }
        return prototype<serializable_type>();
    }

    template<typename serializable_type>
    inline void serializer<serializable_type>::write(const fs::view filePath, const serializable_type &data, const SerializeFormat format)
    {
        std::ofstream os;
        os.open(fs::view_util::get_view_path(filePath, true));
        json j = serialize(data,format);
        os << j.dump(4);
        os.close();
    }

    template<typename serializable_type>
    inline serializable_type serializer<serializable_type>::read(const fs::view filePath, const SerializeFormat format)
    {
        json j;
        std::ifstream is;
        is.open(fs::view_util::get_view_path(filePath, true));
        is >> j;
        is.close();
        return from_reflector(deserialize(j,format));
    }
}
