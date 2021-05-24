#include <core/serialization/serializer.hpp>
#pragma once


namespace legion::core::serialization
{
    template<typename type>
    inline std::unique_ptr<prototype_base> serialization::serializer<type>::deserialize(fs::view filePath)
    {
        return  json_view::deserialize<type>(filePath);
    }

    template<typename type>
    inline void serialization::serializer<type>::serialize(fs::view filePath)
    {
        std::ofstream os;
        os.open(filePath.get_virtual_path());
        os << json_view::serialize<type>(data).dump();
        os.close();
    }   
}
