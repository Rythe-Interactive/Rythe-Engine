#include "filesystem_resolver.hpp"

#include "artifact_cache.hpp"

namespace args::core::filesystem {

    A_NODISCARD file_traits filesystem_resolver::resolve(const std::string& path) noexcept
    {
        set_target(path);
        return get_traits();
    }

    file_traits filesystem_resolver::get_traits() noexcept
    {
        file_traits traits;

        traits.is_file = is_file();
        traits.is_directory = is_directory();
        traits.is_valid = is_valid();
        traits.can_be_written = writeable();
        traits.can_be_read = readable();
        traits.can_be_created = creatable();
        traits.exists = exists();

        inherit_traits(get_fs_traits(), traits);
        sanitize_traits(traits);

        return traits;
    }


    common::result<basic_resource, fs_error> filesystem_resolver::get() const noexcept
    {
        return get(interfaces::implement_signal_t{});
    }

    bool filesystem_resolver::set(const basic_resource& res)
    {

        //TODO(algo-ryth-mix): create strategy to bulk-flush data instead of flushing every time!
        return set(interfaces::implement_signal_t{},res);
    }

    void filesystem_resolver::erase() const noexcept
    {
        erase(interfaces::implement_signal_t{});
    }

}
