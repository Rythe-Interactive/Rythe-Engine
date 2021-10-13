#pragma once
#include <memory>

#include <string_view>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <vector>

#include "detail/meta.hpp"
#include "filesystem_resolver.hpp"
#include "mem_filesystem_resolver.hpp"

namespace legion::core::filesystem{
    class provider_registry
    {
    public:
        using domain = std::string;
        using resolver = filesystem_resolver;
        using resolver_ptr = resolver*;
        
        static std::unordered_set<domain> domains();
        static bool has_domain(domain);

        static void domain_add_resolver(domain, resolver_ptr);

        //TODO(algo-ryth-mix): removed multiple registration, use unordered_map instead of unordered_multimap 
        //TODO(algo-ryth-mix): add checking that only tl resolvers can be of the non-memory variety
        /** @brief Registers a new provider in the registry, which will than be used in searching.
         *  @tparam Resolver The type of the resolver you want to register.
         *  @param d The domain you want to register the provider for.
         *  @note Although multiple registration is currently supported, only the first provider will be used for resolution
         *        multiple registration will be removed in the future.
         *  @params args Additionally forwarded arguments for the creation of Resolver.
         *  @note When the provider is not top level it must be of type mem_filesystem_provider.
         *  
         */
        template <typename Resolver,
                  typename ... Args
            CNDOXY(typename = typename std::enable_if<std::is_base_of<filesystem_resolver_common_base,Resolver>::value>::type)>
        static void domain_create_resolver(domain d,Args&&... args)
        {
            OPTICK_EVENT();
            return domain_add_resolver(d,new Resolver(std::forward<Args>(args)...));
        }

        /** @brief Returns all resolvers associated with the domain.
         */
        static std::vector<resolver_ptr> domain_get_resolvers(domain);


        //NOTICE! this interface is going to be removed, do not use it
        class resolver_sentinel
        {
            friend class provider_registry;
            domain inspected_domain;
            std::size_t index;
            static constexpr std::size_t sentinel_value = std::numeric_limits<std::size_t>::max();

            resolver_sentinel(std::nullptr_t) : index{sentinel_value}{}
            resolver_sentinel(size_t idx, domain d) : inspected_domain(d), index(idx){}
            
        public:
            resolver_sentinel operator++() const;
            resolver_sentinel operator--() const;
            resolver_ptr operator*() const;
            resolver_ptr operator->() const;
            
            friend bool operator==(const resolver_sentinel& lhs, const resolver_sentinel& rhs);
        };

        /** @brief returns the first resolver associated with the domain
         */
        static resolver_sentinel domain_get_first_resolver(domain);

        /** @brief use resolver_sentinel::operator-- instead
         */
        static resolver_sentinel domain_get_prev_resolver(const resolver_sentinel& iterator);

        /** @brief use resolver_sentinel::operator++ instead
         */
        static resolver_sentinel domain_get_next_resolver(const resolver_sentinel& iterator);

        /** @brief returns a resolver_sentinel that points to the end - 1
         *  @note can only be used to check if a domain has reached the end! 
         */
        static resolver_sentinel domain_get_resolver_end();

        /** @brief use resolver_sentinel::operator* instead */
        static resolver_ptr      domain_get_resolver_at(const resolver_sentinel& iterator);
        

    private:

        struct driver;

        provider_registry() = default;
        static driver& get_driver();        
    };
}
