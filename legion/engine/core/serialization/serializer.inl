#include <core/serialization/serializer.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json serializer<type>::serialize(const type data)
    {
        return json_view<prototype<type>>().serialize(prototype<type>(data));
    }

    template<typename type>
    inline prototype<type> serializer<type>::deserialize(json j)
    {
        return json_view<prototype<type>>().deserialize(j);
    }

    //template<>
    //inline prototype<ecs::entity> serializer<ecs::entity>::deserialize(json j)
    //{
    //    return json_view<prototype<ecs::entity>>().deserialize(j);
    //}

    //default will be component
    //template<typename type>
    //inline prototype<ecs::component<type>> serializer<type>::deserialize(json j,int i)
    //{
    //    return json_view<ecs::component<type>>().deserialize(j);
    //}


    template<typename component_type>
    inline void serializer<component_type>::write(fs::view filePath, component_type data)
    {
        std::ofstream os;
        os.open(fs::view_util::get_view_path(filePath, true));
        json j = serialize(data);
        os << j.dump(4);
        os.close();
    }

    template<typename component_type>
    inline component_type serializer<component_type>::read(fs::view filePath)
    {
        json j;
        std::ifstream is;
        is.open(fs::view_util::get_view_path(filePath, true));
        is >> j;
        is.close();
        return from_reflector(deserialize(j));
    }
}
