#include <memory>

#include <string_view>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "detail/meta.hpp"
#include "filesystem_resolver.hpp"
#include "mem_filesystem_resolver.hpp"

namespace args::core::filesystem{
    class provider_registry
    {
    public:
        using domain = std::string_view;
        using resolver = filesystem_resolver;
        using resolver_ptr = resolver*;
        
        static std::unordered_set<domain> domains();
        static bool has_domain(domain);

        static void domain_add_resolver(domain, resolver_ptr);

        template <typename Resolver,
                  typename ... Args,
                  typename = typename std::enable_if<std::is_base_of<filesystem_resolver_common_base,Resolver>::value>::type>
        static void domain_create_resolver(domain d,Args&&... args)
        {
            return domain_add_resolver(d,new Resolver(std::forward<Args>(args)...));
        }

        static std::vector<resolver_ptr> domain_get_resolvers(domain);

        class resolver_sentinel
        {
            friend class provider_registry;
            domain inspected_domain;
            std::size_t index;
            static constexpr std::size_t sentinel_value = std::numeric_limits<std::size_t>::max();

            resolver_sentinel(std::nullptr_t) : index{sentinel_value}{}
            resolver_sentinel(size_t index, domain d) : inspected_domain(d), index(index){}
            
        public:
            resolver_sentinel operator++() const;
            resolver_sentinel operator--() const;
            resolver_ptr operator*() const;
            resolver_ptr operator->() const;
            
            friend bool operator==(const resolver_sentinel& lhs, const resolver_sentinel& rhs);
        };

        static resolver_sentinel domain_get_first_resolver(domain);
        
        static resolver_sentinel domain_get_prev_resolver(const resolver_sentinel& iterator);
        static resolver_sentinel domain_get_next_resolver(const resolver_sentinel& iterator);

        static resolver_sentinel domain_get_resolver_end();

        static resolver_ptr		 domain_get_resolver_at(const resolver_sentinel& iterator);
        

    private:

        provider_registry() = default;
        static provider_registry& get_driver();
        
        std::unordered_multimap<domain,std::unique_ptr<resolver>> m_domain_resolver_map;
        
    };
}