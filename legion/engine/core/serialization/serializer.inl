#include <core/serialization/serializer.hpp>
#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json serializer<type>::serialize(type data)
    {
        return json_view<type>().serialize(data);
    }

    template<typename type>
    inline prototype_base serializer<type>::deserialize(json j)
    {
        return json_view<type>().deserialize(j);
    }

    template<typename type>
    inline void serializer<type>::write(fs::view filePath, type data)
    {
        std::ofstream os;
        os.open(filePath.get_virtual_path());
        json j = serialize(data);
        os << j.dump();
        os.close();
    }

    template<typename type>
    inline prototype_base serializer<type>::read(fs::view filePath)
    {
        //std::ifstream is;
        //is.open(filePath.get_virtual_path());
        //json_view::deserialize<type>(filePath);
        return prototype<type>();
    }
}
