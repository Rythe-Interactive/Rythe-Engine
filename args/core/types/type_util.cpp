#include <core/types/type_util.hpp>

namespace args::core
{
	id_type ARGS_FUNC nameHash(cstring name)
	{
		id_type hash = 0xcbf29ce484222325;
		uint64 prime = 0x00000100000001b3;
		for (int i = 0; i < std::strlen(name); i++)
		{
			byte value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	id_type ARGS_FUNC nameHash(std::string name)
	{
		id_type hash = 0xcbf29ce484222325;
		uint64 prime = 0x00000100000001b3;
		for (int i = 0; i < name.length(); i++)
		{
			byte value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}
}