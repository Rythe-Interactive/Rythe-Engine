#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
#include <core/types/primitives.hpp>
#include <core/async/readonly_rw_spinlock.hpp>

namespace args::core::filesystem
{

	constexpr std::size_t gc_interval = 5;
    constexpr std::size_t gc_keep = 35;
    constexpr std::size_t gc_hist = 10;
	class artifact_cache
	{
	public:
		static std::shared_ptr<byte_vec> get_cache(std::string_view identifier,std::size_t size_hint = 0);
        static artifact_cache& get_driver();
        void gc();


	private:
        artifact_cache() = default;

	    auto& get_caches()
        {
            return m_caches;
        }
        size_t decrease_gci()
        {
            const size_t countdown = m_gc_countdown.fetch_sub(1);
            return countdown-1;
        }

        std::atomic<int32_t> current_mean;

        std::unordered_map<std::string_view,std::pair<std::shared_ptr<byte_vec>,int32_t>> m_caches;
        std::atomic<std::size_t> m_gc_countdown = gc_interval;
        mutable async::readonly_rw_spinlock m_big_gc_lock;
	};
}
