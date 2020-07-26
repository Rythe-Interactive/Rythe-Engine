#include "filesystem_resolver.hpp"

#include "artifact_cache.hpp"

namespace args::core::filesystem {

    A_NODISCARD file_traits filesystem_resolver::resolve(const std::string& path) noexcept
    {
        set_target(path);
        file_traits traits;

        traits.is_file = is_file();
        traits.is_directory = is_directory();
        traits.is_valid = is_valid();
        traits.can_be_written = writeable();
        traits.can_be_read = readable();
        traits.can_be_created = creatable();
        traits.exists = exists();

        inherit_traits(get_traits(), traits);
        sanitize_traits(traits);

        return traits;
    }


    common::result<basic_resource, fs_error> filesystem_resolver::get() const noexcept
    {
        using common::Ok;

        if(in_cache())
        {
            byte_vec v;
            load_cache(&v);
            return Ok(basic_resource(v));
        }
        return get(interfaces::implement_signal_t{});
    }

    bool filesystem_resolver::set(const basic_resource& res)
    {
        save_cache(&res.get());

         //TODO(algo-ryth-mix): create strategy to bulk-flush data instead of flushing every time!
        return set(interfaces::implement_signal_t{},res);
    }

    void filesystem_resolver::erase() const noexcept
    {
        artifact_cache::ping_identifier(generate_artifact_identifier(),ac_ping_operation::DELETE);
        erase(interfaces::implement_signal_t{});

    }

    bool filesystem_resolver::in_cache() const
    {
        return artifact_cache::is_cached(generate_artifact_identifier());
    }

    void filesystem_resolver::load_cache(byte_vec* data) const
    {
        artifact_cache::get_data(generate_artifact_identifier(),data);
    }

    void filesystem_resolver::save_cache(const byte_vec* data) const
    {
        artifact_cache::set_data(generate_artifact_identifier(),data);
    }
    std::string filesystem_resolver::generate_artifact_identifier() const
    {
        return m_identifier + artifact_cache::ident_target_delimiter + m_target;
    }
}
