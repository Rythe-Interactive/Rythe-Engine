#pragma once
#include "filesystem_resolver.hpp"


namespace args::core::filesystem
{
	class mem_filesystem_resolver : filesystem_resolver
	{
	public:
		bool seek_data();

		A_NODISCARD bool has_data_ready() const;
		void set_data(const byte_vec& d);		

	protected:

		A_NODISCARD const byte_vec& get_data() const;
		A_NODISCARD byte_vec& get_data();

	private:

        std::string m_identifier;
		byte_vec m_data;
	};
}
