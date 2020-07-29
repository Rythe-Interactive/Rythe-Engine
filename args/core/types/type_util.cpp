#include <core/types/type_util.hpp>

namespace args::core
{
	id_type ARGS_FUNC nameHash(cstring name)
	{
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < std::strlen(name); i++)
		{
			byte value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}

	id_type ARGS_FUNC nameHash(string name)
	{
		id_type hash = 0;

		hash = 0x811c9dc5;
		uint32 prime = 0x1000193;
		for (int i = 0; i < name.length(); i++)
		{
			byte value = name[i];
			hash = hash ^ value;
			hash *= prime;
		}

		return hash;
	}
}