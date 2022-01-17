#include <core/serialization/serializer_views/bson.hpp>

namespace legion::core::serialization
{

    common::result<void, fs_error> bson::write(fs::view& file)
    {
        return file.set(fs::basic_resource(nlohmann::ordered_json::to_bson(root.item)));
    }

    common::result<void, fs_error> bson::read(const fs::view& file)
    {
        auto result = file.get();
        if (!result.valid())
        {
            log::error(result.error().what());
            return result.error();
        }

        root.key = "root";
        root.item = nlohmann::ordered_json::from_bson(result->get());
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> bson::read(const byte_vec& data)
    {
        root.key = "root";
        root.item = nlohmann::ordered_json::from_bson(data);
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> bson::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        root.key = "root";
        root.item = nlohmann::ordered_json::from_bson(begin, end);
        root.currentReadIndex = 0;
        return common::success;
    }
}

