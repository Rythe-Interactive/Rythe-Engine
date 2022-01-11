#include <core/serialization/serializer_views/serializer_view.hpp>

namespace legion::core::serialization
{
    bool serializer_view::serialize(const std::string& name, const void* value, id_type typeId)
    {
        if (typeId == typeHash<int>())
        {
            serialize_int(name, *static_cast<const int*>(value));
            return true;
        }
        else if (typeId == typeHash<float>())
        {
            serialize_float(name, *static_cast<const float*>(value));
            return true;
        }
        else if (typeId == typeHash<double>())
        {
            serialize_double(name, *static_cast<const double*>(value));
            return true;
        }
        else if (typeId == typeHash<bool>())
        {
            serialize_bool(name, *static_cast<const bool*>(value));
            return true;
        }
        else if (typeId == typeHash<std::string>())
        {
            serialize_string(name, *static_cast<const std::string*>(value));
            return true;
        }
        else if (typeId == typeHash<id_type>())
        {
            serialize_id_type(name, *static_cast<const id_type*>(value));
            return true;
        }
        return false;
    }

    common::result<void, fs_error> serializer_view::deserialize(const std::string& name, void* value, id_type typeId)
    {
        if (typeId == typeHash<int>())
        {
            auto result = deserialize_int(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<int*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }
        else if (typeId == typeHash<float>())
        {
            auto result = deserialize_float(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<float*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }
        else if (typeId == typeHash<double>())
        {
            auto result = deserialize_double(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<double*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }
        else if (typeId == typeHash<bool>())
        {
            auto result = deserialize_bool(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<bool*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }
        else if (typeId == typeHash<std::string>())
        {
            auto result = deserialize_string(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<std::string*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }
        else if (typeId == typeHash<id_type>())
        {
            auto result = deserialize_id_type(name);
            if (result.has_error())
                return { result.error(), result.warnings() };
            else
            {
                *static_cast<id_type*>(value) = result.value();
                return { common::success, result.warnings() };
            }
        }

        return legion_fs_error("Type was not a primitive serializable type.");
    }
}
