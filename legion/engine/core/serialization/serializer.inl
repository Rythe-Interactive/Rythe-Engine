#include <core/serialization/serializer.hpp>
#include <core/serialization/serializer_view.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename serializable_type>
    inline json serializer<serializable_type>::serialize(const serializable_type data)
    {
        return serializer_view<prototype<serializable_type>>().serialize(prototype<serializable_type>(data));
    }

    template<typename serializable_type>
    inline prototype<serializable_type> serializer<serializable_type>::deserialize(json j)
    {
        return serializer_view<prototype<serializable_type>>().deserialize(j);
    }

    template<typename serializable_type>
    inline void serializer<serializable_type>::write(fs::view filePath, serializable_type data)
    {
        std::ofstream os;
        os.open(fs::view_util::get_view_path(filePath, true));
        json j = serialize(data);
        os << j.dump(4);
        os.close();
    }

    template<typename serializable_type>
    inline serializable_type serializer<serializable_type>::read(fs::view filePath)
    {
        json j;
        std::ifstream is;
        is.open(fs::view_util::get_view_path(filePath, true));
        is >> j;
        is.close();
        return from_reflector(deserialize(j));
    }
}
