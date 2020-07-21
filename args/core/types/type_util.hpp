#pragma once
#include <typeinfo>
#include <core/types/primitives.hpp>

/**
 * @file type_util.hpp
 */

namespace args::core
{
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
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < N - 1; i++)
		{
			byte_t value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	/**@todo find a way to make this down here compile without fucking shit up cuz redefinition.
	*/
#ifndef NON_TEMPLATE_NAMEHASH
	#define NON_TEMPLATE_NAMEHASH

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	/*id_type nameHash(cstring name)
	{
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < strlen(name); i++)
		{
			byte_t value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}*/

	/**@brief Returns hash of a certain string
	 * @param name Name you wish to hash
	 */
	/*id_type nameHash(string name)
	{
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < name.length(); i++)
		{
			byte_t value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}*/

#endif // !NON_TEMPLATE_NAMEHASH

	/**@brief Returns hash of the type name.
	 * @tparam T type of which you want the hash.
	 */
	template<typename T>
	constexpr id_type typeHash()
	{
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		cstring name = typeName<T>();
		for (int i = 0; i < strlen(name); i++)
		{
			byte_t value = name[i];
			hash = hash ^ value;
			hash *= prime;
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