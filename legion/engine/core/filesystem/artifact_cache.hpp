#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
#include <core/types/primitives.hpp>
#include <core/async/rw_spinlock.hpp>

namespace legion::core::filesystem
{

    constexpr std::size_t gc_interval = 5;
    constexpr std::size_t gc_keep = 35;
    constexpr std::size_t gc_hist = 10;


    /**@class artifact_cache
     * @brief Manages caches for `mem_filesystem_provider`.
     * @note  This class is not exported! This should only be used by library components.
     */
    class artifact_cache
    {
    public:

        /**@brief Queries a cache for a `mem_filesystem_provider`.
         * 
         * @param identifier Provider identifier.
         * @param size_hint  A hint to how big the cache is going to be. 
         * @return shared_ptr to a byte_vec Which should be used as the cache.
         * @ref mem_filesystem_provider::build_memory_representation
         */
        static std::shared_ptr<byte_vec> get_cache(const std::string& identifier, std::size_t size_hint = 0);

        /**@brief Gets the singleton driver for the artifact_cache.
         */
        static artifact_cache& get_driver();

        /**@brief Manually calls garbage collection.
         */
        void gc();

    private:
        artifact_cache() = default;

        auto& get_caches()
        {
            return m_caches;
        }
        size_t decrease_gcc()
        {
            const size_t countdown = m_gc_countdown.fetch_sub(1);
            return countdown-1;
        }

        std::atomic<int32_t> current_mean;

        std::unordered_map<std::string, std::pair<std::shared_ptr<byte_vec>, int32_t>> m_caches;
        std::atomic<std::size_t> m_gc_countdown = { gc_interval };
        mutable async::rw_spinlock m_big_gc_lock;
    };
}
