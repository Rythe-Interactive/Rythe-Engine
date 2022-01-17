#pragma once
#include <core/serialization/serializer_views/json.hpp>

namespace legion::core::serialization
{
    struct bson : public json
    {
        bson() = default;
        virtual ~bson() = default;

        L_NODISCARD virtual common::result<void, fs_error> write(fs::view& file) override;
        L_NODISCARD virtual common::result<void, fs_error> read(const fs::view& file) override;
        L_NODISCARD virtual common::result<void, fs_error> read(const byte_vec& data) override;
        L_NODISCARD virtual common::result<void, fs_error> read(byte_vec::iterator begin, byte_vec::iterator end) override;
    };
}
