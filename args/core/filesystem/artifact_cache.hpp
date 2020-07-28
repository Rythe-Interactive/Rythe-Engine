#pragma once
#include <vector>
#include <tuple>
#include <map>


#include <core/types/primitives.hpp>
#include <core/async/readonly_rw_spinlock.hpp>

#include <core/containers/data_view.hpp>

namespace args::core::filesystem
{
	enum class ac_ping_operation
	{
	    ADD_POINTS,
		REMOVE_POINTS,
		DELETE
	};
	constexpr static std::size_t ac_base_score = 10;
	constexpr static std::size_t ac_clean_score = 0;
	constexpr static std::size_t ac_hit_gain = 1;

	class artifact_cache
	{
	public:

		using score_t = std::int8_t;
		using range_begin_t = std::size_t;
		using range_end_t = std::size_t;


		static constexpr const char * ident_target_delimiter = ":";
		static bool is_cached(std::string_view identifier);

		static void ping_identifier(std::string_view identifier,ac_ping_operation op);

		static void set_data(std::string_view identifier, const byte_vec* data);

		static data_view<const byte_t> get_data(std::string_view identifier);

	private:
		mutable std::map<std::string_view,std::tuple<range_begin_t,range_end_t,score_t>> m_locations;
		std::vector<byte_t> m_container;
		mutable async::readonly_rw_spinlock lock;

		template <class Func>
		auto critical_read_section(Func&& f ) const -> decltype(auto)
		{
			async::readonly_guard guard(lock);
			return std::invoke(f,m_container,m_locations);
		}

		template <class Func>
		auto critical_write_section(Func&& f) -> decltype(auto)
		{
			async::readwrite_guard guard(lock);
			return std::invoke(f,m_container,m_locations);
		}

		artifact_cache() = default;
		static artifact_cache& get_driver();
	};
}
