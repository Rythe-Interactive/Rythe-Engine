#pragma once
#include <typeinfo>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>
#include <string>
#include <cstring>

/**
 * @file type_util.hpp
 */

namespace args::core
{
	/**@brief Forcefully casts non pointer/reference type value from one to another.
	*/
	template<typename T, typename U>
	T force_value_cast(U value)
	{
		static_assert(alignof(T) == alignof(U), "Illegal cast of unaligned types.");
		static_assert(sizeof(T) == sizeof(U), "Illegal cast of non size similar types.");

		return *reinterpret_cast<T*>(&value);
	}

	/**@brief Returns typeid(T).name().
	 * @tparam T type of which you want the name.
	 */
	template<typename T>
	cstring typeName()
	{
		static cstring name = nullptr;
		if (!name)
			name = typeid(T).name();
		return name;
	}

	template<typename T>
	cstring undecoratedTypeName()
	{
		static char* name = nullptr;
		if (!name)
		{
			std::string typeName = typeid(T).name();
			size_type token;
			if ((token = typeName.find("struct ")) != std::string::npos)
				typeName = typeName.substr(token + 6);
			else if ((token = typeName.find("class ")) != std::string::npos)
				typeName = typeName.substr(token + 5);

			name = (char*) malloc(typeName.size());
			typeName.copy(name, std::string::npos);
			name[typeName.size()] = '\0';
		}
		return name;
	}

	/**@brief Returns typeid(T).name().
	 * @tparam T type of which you want the name.
	 * @param expr Variable of which you wish to auto deduct type.
	 */
	template<typename T>
	cstring typeName(T expr)
	{
		return typeName<T>();
	}

	/**@brief Returns hash of a certain string
	 * @tparam N Length of the string literal
	 * @param name Name you wish to hash
	 * @note Since this version takes a const char[] it can only really be used with data coming from a string literal.
	 *		 Because it takes in a const char[] this function is able to be constexpr and thus have minimal overhead.
	 */
	template<size_type N>
	constexpr id_type nameHash(const char(&name)[N])
	{
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
	id_type ARGS_FUNC nameHash(cstring name);

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	id_type ARGS_FUNC nameHash(const std::string& name);

	/**@brief Returns hash of the type name.
	 * @tparam T type of which you want the hash.
	 */
	template<typename T>
	id_type typeHash()
	{
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
	constexpr id_type typeHash(T expr)
	{
		return typeHash<T>();
	}
}
