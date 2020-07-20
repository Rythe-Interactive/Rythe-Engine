#pragma once
#include <core/common/common.hpp>

namespace args::core::filesystem {
    struct file_traits {
        bool is_file {};
        bool is_directory {};
        bool is_valid{};

        bool can_be_written{};
        bool can_be_read{};
        bool can_be_created{};

        bool exists{};
    };

    struct filesystem_traits {
        bool is_readonly {};
    };

    inline void inherit_traits(const filesystem_traits& fstraits, file_traits* ftraits)
    {
        assert_msg("ftraits was nullptr",ftraits != nullptr);

        if(fstraits.is_readonly)
        {
            ftraits->can_be_written = false;
            ftraits->can_be_created = false;

        }
    }

    inline void sanitize_traits(file_traits* traits)
    {
        assert_msg("traits was nullptr",traits != nullptr);

        if(traits->is_file) traits->is_directory = false;
        if(traits->is_directory)
        {
            traits->can_be_written = false;
            traits->can_be_read = false;
        }
        if(traits->exists)
        {
            traits->can_be_created = false;
            traits->is_valid = true;
            if(traits->is_file)
            {
                traits->can_be_read = true;
            }
        }
    }
}