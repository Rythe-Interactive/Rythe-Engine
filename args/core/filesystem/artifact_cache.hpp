#pragma once
#include <string>

namespace args::core::filesystem
{
	enum class ac_ping_operation
	{
	    ADD_POINTS,
		REMOVE_POINTS,
		DELETE
	};



	class artifact_cache
	{
	public:
		static constexpr const char * ident_target_delimiter = ":";
		static bool is_cached(const std::string& identifier);

		static void ping_identifier(const std::string& identifier,ac_ping_operation op);

		static void set_data(const std::string& identifier, const byte_vec* data);

		static void get_data(const std::string& identifier, byte_vec* data);

	};
}
