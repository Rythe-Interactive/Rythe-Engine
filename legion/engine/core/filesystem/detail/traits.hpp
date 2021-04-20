#pragma once

namespace legion::core::filesystem {
    struct file_traits {
        bool is_file {};
        bool is_directory {};
        bool is_valid_path{};

        bool can_be_written{};
        bool can_be_read{};
        bool can_be_created{};

        bool exists{};
    };

    constexpr static  file_traits invalid_file_t{false,false,false,false,false,false,false};

    inline bool operator == (const file_traits& rhs, const file_traits& lhs) noexcept
    {
        if(&rhs == &lhs) return true;

        return rhs.is_file == lhs.is_file &&
               rhs.is_directory == lhs.is_directory &&
               rhs.is_valid_path == lhs.is_valid_path &&
               rhs.can_be_written == lhs.can_be_written &&
               rhs.can_be_read == lhs.can_be_read &&
               rhs.can_be_created == lhs.can_be_created &&
               rhs.exists == lhs.exists;
    }

    inline bool operator != (const file_traits& rhs, const file_traits& lhs) noexcept
    {
        return !(rhs == lhs);
    }


    struct filesystem_traits {
        bool is_readonly {};
        bool is_valid {};
    };

    inline void inherit_traits(const filesystem_traits& fstraits, file_traits& ftraits) noexcept
    {
        if(!fstraits.is_valid) 
        {
            ftraits = invalid_file_t;
            return;
        }
        if(fstraits.is_readonly)
        {
            ftraits.can_be_written = false;
            ftraits.can_be_created = false;
        }
    }

    inline void inherit_traits(const filesystem_traits& outer, filesystem_traits& inner) noexcept
    {
        if(!outer.is_valid) inner.is_valid = false;
        if(outer.is_readonly) inner.is_readonly = true;
    }


    constexpr static filesystem_traits invalid_filesystem_t{false,false};

    inline void sanitize_traits(file_traits& traits) noexcept
    {

        if(traits.is_file) traits.is_directory = false;
        if(traits.is_directory)
        {
            traits.can_be_written = false;
            traits.can_be_read = false;
        }
        if(traits.exists)
        {
            traits.can_be_created = false;
            traits.is_valid_path = true;
            if(traits.is_file)
            {
                traits.can_be_read = true;
            }
        }
    }
}
