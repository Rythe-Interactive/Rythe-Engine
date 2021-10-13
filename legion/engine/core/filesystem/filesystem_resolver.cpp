#include "filesystem_resolver.hpp"

#include "artifact_cache.hpp"

namespace legion::core::filesystem {

    L_NODISCARD file_traits filesystem_resolver::resolve(const std::string& path) noexcept
    {
        set_target(path);
        return get_traits();
    }

    file_traits filesystem_resolver::get_traits() noexcept
    {
        //get all traits via the specific functions
        //TODO: this could possibly be optimized by calling inherit_traits

        file_traits traits;

        traits.is_file = is_file();
        traits.is_directory = is_directory();
        traits.is_valid_path = is_valid_path();
        traits.can_be_written = writeable();
        traits.can_be_read = readable();
        traits.can_be_created = creatable();
        traits.exists = exists();

        inherit_traits(get_fs_traits(), traits);
        sanitize_traits(traits);

        return traits;
    }


    common::result<basic_resource, fs_error> filesystem_resolver::get() noexcept
    {
        //this interface might change on a library basis
        //to avoid breaking the code the user has to implement a different method
        return get(interfaces::implement_signal_t{});
    }

    common::result<const basic_resource, fs_error> filesystem_resolver::get() const noexcept
    {
        //this interface might change on a library basis
        //to avoid breaking the code the user has to implement a different method
        return get(interfaces::implement_signal_t{});
    }

    common::result<void, fs_error> filesystem_resolver::set(const basic_resource& res)
    {
        //this interface might change on a library basis
        //to avoid breaking the code the user has to implement a different method
        //TODO(algo-ryth-mix): create strategy to bulk-flush data instead of flushing every time!
        return set(interfaces::implement_signal_t{},res);
    }

    void filesystem_resolver::erase() const noexcept
    {
        //this interface might change on a library basis
        //to avoid breaking the code the user has to implement a different method
        erase(interfaces::implement_signal_t{});
    }

}
