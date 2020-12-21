#pragma once
#include <typeinfo>
#include <core/types/primitives.hpp>
#include <core/types/meta.hpp>
#include <core/platform/platform.hpp>
#include <string>
#include <string_view>
#include <cstring>

#include <Optick/optick.h>

/**
 * @file type_util.hpp
 */

namespace legion::core
{
    /**@brief Forcefully casts non pointer/reference type value from one to another.
    */
    template<typename T, typename U>
    constexpr T force_value_cast(U value)
    {
        static_assert(alignof(T) == alignof(U), "Illegal cast of unaligned types.");
        static_assert(sizeof(T) == sizeof(U), "Illegal cast of non size similar types.");

        return *reinterpret_cast<T*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(const U& value)
    {
        return reinterpret_cast<T*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(U&& value)
    {
        return reinterpret_cast<T*>(&value);
    }

    template<typename T, typename U>
    constexpr T* force_cast(U* value)
    {
        return reinterpret_cast<T*>(value);
    }

    /**@brief Returns typeid(T).name().
     * @tparam T type of which you want the name.
     */
    template<typename T>
    cstring typeName()
    {
        OPTICK_EVENT();
        static cstring name = typeid(T).name();
        return name;
    }

    template<typename T>
    cstring undecoratedTypeName()
    {
        OPTICK_EVENT();
        static std::string name;
        if (name.empty())
        {
            name = std::string(typeid(T).name());
            size_type token;
            if ((token = name.find("struct ")) != std::string::npos)
                name = name.substr(token + 6);
            else if ((token = name.find("class ")) != std::string::npos)
                name = name.substr(token + 5);
        }
        return name.c_str();
    }

    /**@brief Returns typeid(T).name().
     * @tparam T type of which you want the name.
     * @param expr Variable of which you wish to auto deduct type.
     */
    template<typename T>
    cstring typeName(T expr)
    {
        OPTICK_EVENT();
        return typeid(expr).name();
    }

    /**@brief Returns hash of a certain string
     * @tparam N Length of the string literal
     * @param name Name you wish to hash
     * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
     *		 Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
     */
    template<size_type N>
    id_type nameHash(const char(&name)[N])
    {
        OPTICK_EVENT();
        id_type hash = 0xcbf29ce484222325;
        uint64 prime = 0x00000100000001b3;
        for (int i = 0; i < N - 1; i++)
        {
            byte value = name[i];
            hash = hash ^ value;
            hash *= prime;
        }

        return hash;
    }

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    id_type LEGION_FUNC nameHash(cstring name);

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    id_type LEGION_FUNC nameHash(const std::string& name);

    /**@brief Returns hash of a certain string
     * @param name Name you wish to hash
     */
    id_type LEGION_FUNC nameHash(const std::string_view& name);

    /**@brief Returns hash of the type name.
     * @tparam T type of which you want the hash.
     */
    template<typename T>
    id_type typeHash()
    {
        OPTICK_EVENT();
        static id_type hash = 0;

        if (hash == 0)
        {
            hash = 0xcbf29ce484222325;
            uint64 prime = 0x00000100000001b3;
            cstring name = undecoratedTypeName<T>();
            while (*name != '\0')
            {
                hash = hash ^ (byte)*name;
                hash *= prime;
                name++;
            }
        }

        return hash;
    }

    /**@brief Returns hash of the type name.
     * @tparam T type of which you want the hash.
     * @param expr Variable of which you wish to auto deduct type.
     */
    template<typename T>
    id_type typeHash(T expr)
    {
        OPTICK_EVENT();
        return typeHash<T>();
    }

    template<typename Iterator>
    void appendBinaryData(Iterator first, Iterator last, byte_vec& data);

    template<typename T>
    void appendBinaryData(T* value, byte_vec& data)
    {
        OPTICK_EVENT();
        if constexpr (has_resize<std::remove_const_t<T>, void(const std::size_t)>::value)
        {
            auto first = value->begin();
            auto last = value->end();

            uint64 arrSize = std::distance(first, last) * sizeof(typename decltype(first)::value_type);

            for (int i = 0; i < sizeof(uint64); i++)
                data.push_back(reinterpret_cast<const byte*>(&arrSize)[i]);

            for (auto it = first; it != last; ++it)
                appendBinaryData(&*it, data);
        }
        else
        {
            for (int i = 0; i < sizeof(T); i++)
                data.push_back(reinterpret_cast<const byte*>(value)[i]);
        }
    }

    template<typename Iterator>
    void appendBinaryData(Iterator first, Iterator last, byte_vec& data)
    {
        OPTICK_EVENT();
        uint64 arrSize = std::distance(first, last) * sizeof(typename Iterator::value_type);
        appendBinaryData(&arrSize, data);

        for (Iterator it = first; it != last; ++it)
            appendBinaryData(&*it, data); // dereference iterator to get reference, then get the address to get a pointer.
    }

    template<typename T>
    void retrieveBinaryData(T& value, byte_vec::const_iterator& start);

    template<typename Iterator>
    void retrieveBinaryData(Iterator first, Iterator last, byte_vec::const_iterator& start);

    template<typename T>
    uint64 retrieveArraySize(byte_vec::const_iterator start)
    {
        OPTICK_EVENT();
        uint64 arrSize;
        retrieveBinaryData(arrSize, start);
        if (arrSize % sizeof(T) == 0)
            return arrSize / sizeof(T);
        return 0;
    }

    template<typename T>
    void retrieveBinaryData(T& value, byte_vec::const_iterator& start)
    {
        OPTICK_EVENT();
        if constexpr (has_resize<T, void(std::size_t)>::value)
        {
            uint64 arrSize = retrieveArraySize<typename T::value_type>(start);
            value.resize(arrSize);

            retrieveBinaryData(value.begin(), value.end(), start);
        }
        else
        {
            memcpy(&value, &*start, sizeof(T));

            start += sizeof(T);
        }
    }

    template<typename Iterator>
    void retrieveBinaryData(Iterator first, Iterator last, byte_vec::const_iterator& start)
    {
        OPTICK_EVENT();
        uint64 arrSize;
        retrieveBinaryData(arrSize, start);

        uint64 dist = std::distance(first, last) * sizeof(typename Iterator::value_type);
        if (dist > arrSize)
            dist = arrSize;

        Iterator valueIt = first;

        for (byte_vec::const_iterator it = start; it != (start + dist); ++valueIt)
        {
            retrieveBinaryData(*valueIt, it);
        }

        start += arrSize;
    }

}
