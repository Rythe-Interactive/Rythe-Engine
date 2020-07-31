#pragma once
#include <typeinfo>
#include <core/types/primitives.hpp>
#include <core/platform/platform.hpp>

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
		static_assert(alignof(T) == alignof(U), "Illegal cast of unaligned types. Approaching undefined behavior.");
		static_assert(sizeof(T) == sizeof(U), "Illegal cast of non size similar types. Approaching undefined behavior.");

		return *reinterpret_cast<T*>(&value);
	}

	/**@brief Returns typeid(T).name().
	 * @tparam T type of which you want the name.
	 */
	template<typename T>
	constexpr cstring typeName()
	{
		return typeid(T).name();
	}

	/**@brief Returns typeid(T).name().
	 * @tparam T type of which you want the name.
	 * @param expr Variable of which you wish to auto deduct type.
	 */
	template<typename T>
	constexpr cstring typeName(T expr)
	{
		return typeid(T).name();
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
		id_type hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < N - 1; i++)
		{
			byte value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	/**@todo use ARGS_FUNC
	*/

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	id_type ARGS_FUNC nameHash(cstring name);

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	id_type ARGS_FUNC nameHash(std::string name);

	/**@brief Returns hash of the type name.
	 * @tparam T type of which you want the hash.
	 */
	template<typename T>
	constexpr id_type typeHash()
	{
		id_type hash = 0xcbf29ce484222325;
		uint64 prime = 0x00000100000001b3;
		cstring name = typeid(T).name();
		while (*name != '\0')
		{
			hash = hash ^ (byte)*name;
			hash *= prime;
			name++;
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