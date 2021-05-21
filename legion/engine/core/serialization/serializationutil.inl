#include <core/serialization/serializationutil.hpp>
#pragma once


namespace legion::core::serialization
{
    template<typename type>
    inline void serialization::serializer<type>::read(fs::view filePath)
    {
        data = json_view::deserialize<type>(filePath);
    }

    template<typename type>
    inline void serialization::serializer<type>::write(fs::view filePath)
    {
        std::ofstream os;
        os.open(filePath.get_virtual_path());
        os << json_view::serialize<type>(data).dump();
        os.close();
    }

    template<typename type>
    inline void serialization::serializer<type>::store(const type& t)
    {
        data = t;
    }
    template<typename type>
    inline std::unique_ptr<prototype_base> serialization::serializer<type>::load()
    {
        return std::unique_ptr<prototype<type>>();
    }


    template<typename type>
    static inline json serialization::json_view::serialize(type t)
    {
        component_prototype<type> temp = component_prototype<type>(t);
        json j;
        j["Type ID"] = type_hash<type>().local();
        return j;
    }

    /**@brief JSON deserialization from a string
     * @param json the input JSON string
     * @returns the the deserialized object as type
     */
    template<typename type>
    static inline type serialization::json_view::deserialize(fs::view filePath)
    {
        type t;
        return t;
    }
}
