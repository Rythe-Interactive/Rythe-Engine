#include "filesystem_resolver.hpp"

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
}