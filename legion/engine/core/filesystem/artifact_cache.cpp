#include <core/filesystem/artifact_cache.hpp>
#include <algorithm>
#include <functional>
#include <core/containers/iterator_tricks.hpp>

namespace legion::core::filesystem {
    std::shared_ptr<byte_vec> artifact_cache::get_cache(const std::string& identifier, std::size_t size_hint)
    {
        std::shared_ptr<byte_vec> result;

        static auto& driver = get_driver();
        {
            async::readonly_guard guard(driver.m_big_gc_lock);

            //query provider
            auto& [ptr, score] =  driver.get_caches()[identifier];
            if(!ptr)
            {
                //prepare new provider
                score = driver.current_mean.load();
                if(size_hint)
                    ptr = std::make_shared<byte_vec>(size_hint);
                else
                    ptr = std::make_shared<byte_vec>();
            }
            else
            {
                //bump existing provider
                score++;
            }

            result = ptr;
        }

        //run gc from time to time
        if(driver.decrease_gcc() == 0) driver.gc();
        return result;
    }

    artifact_cache& artifact_cache::get_driver() {
        static artifact_cache cache{};
        return cache;
    }

    void artifact_cache::gc()
    {
        m_gc_countdown = gc_interval;

        //nothing to gc, still below threshold
        if(m_caches.size() < (gc_keep + gc_hist)) return;

        //create heap for finding the highest N
        std::vector<std::int32_t> score_heap;

        //avoid writing two basically self similar loops
        auto composer = [this](auto&& action)
        {
            //iterate over all elements in the cache
            for(auto&[ptr, score] : values_only(m_caches))
            {
                //check if element should be skipped because it is to new or still used somewhere
                if(ptr.use_count() > 1 || ptr->empty()) continue;

                //invoke custom action
                std::invoke(action,ptr,score);
            }
        };

        //we abuse this read-write lock a bit here to avoid creation of new
        //elements while gc is running, such that we do not insert and remove simultaneously
        async::readwrite_guard guard(m_big_gc_lock);

        composer([&score_heap](L_MAYBEUNUSED auto& p,auto score)
        {
            //push all scores into a heap for sorting
            score_heap.emplace_back(score);
            std::push_heap(score_heap.begin(),score_heap.end());
        });

        //sort scores
        std::sort_heap(score_heap.begin(),score_heap.end());

        //find lowest element that should not be removed
        const auto low_element = *(score_heap.end()-gc_keep);
        current_mean = low_element;

        composer([low_element](auto& ptr,auto score){
            //delete element if it is below threshold
            if(score - low_element < 0)
                ptr.reset();
        });
    }
}
