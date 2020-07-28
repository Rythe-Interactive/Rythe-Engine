#include "artifact_cache.hpp"


namespace args::core::filesystem {
    
    using locations_t = std::map<std::string_view,std::tuple<
        artifact_cache::range_begin_t,
        artifact_cache::range_end_t,
        artifact_cache::score_t
        >
    >;
    using data_t = std::vector<byte_t>;

    using cdata_t = const data_t;


    artifact_cache& artifact_cache::get_driver() {
        static artifact_cache cache{};
        return cache;
    }

    bool artifact_cache::is_cached(std::string_view identifier)
    {
        static artifact_cache& driver = get_driver();
     
        //for checking if something is in cache we only need a read context
        return driver.critical_read_section(
            [&identifier] (cdata_t& data, locations_t&clocs)
            {
                //we don't care about data, we just need to know if the element exists in the map
                (void)data;

                //check if the key exists and if its score is bigger than 0
                if(const auto it = clocs.find(identifier);it != clocs.end())
                {
                    const auto& [begin,end,score] = it->second;
                    return score > 0;
                }
                return false;
            }
        );
    }

    data_view<const byte_t> artifact_cache::get_data(std::string_view identifier)
    {
        static artifact_cache& driver = get_driver();
        
        return driver.critical_read_section(
            [&identifier](cdata_t& data,locations_t& clocs)
            {
                if(const auto it = clocs.find(identifier); it != clocs.end())
                {
                    const auto& [begin,end,score] = it->second;

                    const auto* ptr = data.data();

                    it->second = std::make_tuple(begin,end,score+1);

                    return data_view<const byte_t>(ptr,end-begin,begin,false);
                }
                else
                {
                    return data_view<const byte_t>(nullptr,0);
                }
            }
        );
    }

}
