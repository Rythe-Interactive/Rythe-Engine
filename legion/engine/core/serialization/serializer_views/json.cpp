#include <core/serialization/serializer_views/json.hpp>

namespace legion::core::serialization
{
    json::entry& json::current_item()
    {
        if (isPeaking)
            return peakObject;
        return active_stack.top();
    }

    void json::serialize_int(const std::string& name, int serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }

    void json::serialize_float(const std::string& name, float serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }

    void json::serialize_double(const std::string& name, double serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }

    void json::serialize_bool(const std::string& name, bool serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }

    void json::serialize_string(const std::string& name, const std::string& serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }

    void json::serialize_id_type(const std::string& name, id_type serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.is_object())
            item.emplace(name, serializable);
        else if (item.is_array())
            item.emplace_back(serializable);
    }


    common::result<int, fs_error> json::deserialize_int(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_number_integer())
                return legion_fs_error("Item with name: " + name + " is not an integer number.");

            return iterator->get<int>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_number_integer())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not an integer number.");

            readIndex++;
            return item.get<int>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    common::result<float, fs_error> json::deserialize_float(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_number_float())
                return legion_fs_error("Item with name: " + name + " is not a floating point number.");

            return iterator->get<float>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_number_float())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not a floating point number.");

            readIndex++;
            return item.get<float>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    common::result<double, fs_error> json::deserialize_double(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_number_float())
                return legion_fs_error("Item with name: " + name + " is not a floating point number.");

            return iterator->get<double>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_number_float())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not a floating point number.");

            readIndex++;
            return item.get<double>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    common::result<bool, fs_error> json::deserialize_bool(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_boolean())
                return legion_fs_error("Item with name: " + name + " is not a boolean.");

            return iterator->get<bool>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_boolean())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not a boolean.");

            readIndex++;
            return item.get<bool>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    common::result<std::string, fs_error> json::deserialize_string(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_string())
                return legion_fs_error("Item with name: " + name + " is not a string.");

            return iterator->get<std::string>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_string())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not a string.");

            readIndex++;
            return item.get<std::string>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    common::result<id_type, fs_error> json::deserialize_id_type(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.is_object())
        {
            auto iterator = current.find(name);
            if (iterator == current.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_number_integer())
                return legion_fs_error("Item with name: " + name + " is not an integer number.");

            return iterator->get<id_type>();
        }
        else if (current.is_array())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading json beyond the size of the array.");
            auto& item = current.at(readIndex);

            if (!item.is_number_integer())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in json array " + currentName + " is not an integer number.");

            readIndex++;
            return item.get<id_type>();
        }
        return legion_fs_error("Current json object is invalid.");
    }

    void json::start_object()
    {
        active_stack.emplace("item_" + std::to_string(current_item().item.size()), nlohmann::ordered_json::object());
    }

    void json::start_object(const std::string& name)
    {
        active_stack.emplace(name, nlohmann::ordered_json::object());
    }

    void json::end_object()
    {
        if (active_stack.empty())
            return;

        auto& [name, object, readIndex] = current_item();

        if (!object.is_object())
            return;

        if (active_stack.size() == 1)
        {
            root.item.emplace(name, object);
            active_stack.pop();
        }
        else
        {
            entry cpy{ name, object, readIndex };
            active_stack.pop();
            auto& [nxtName, next, nxtRdIdx] = current_item();

            if (next.is_array())
                next.push_back(cpy.item);
            else if (next.is_object())
                next.emplace(cpy.key, cpy.item);
        }
    }

    void json::start_container(const std::string& name)
    {
        active_stack.emplace(name, nlohmann::ordered_json::array());
    }

    void json::end_container()
    {
        if (active_stack.empty())
            return;

        auto& [name, arr, readIndex] = current_item();

        if (!arr.is_array())
            return;

        if (active_stack.size() == 1)
        {
            root.item.emplace(name, arr);
            active_stack.pop();
        }
        else
        {
            entry cpy{ name, arr, readIndex };
            active_stack.pop();
            auto& [nxtName, next, nxtRdIdx] = current_item();

            if (next.is_array())
                next.push_back(arr);
            else if (next.is_object())
                next.emplace(cpy.key, cpy.item);
        }
    }

    common::result<void, fs_error> json::start_read(const std::string& name)
    {
        isPeaking = false;
        if (active_stack.empty())
        {
            auto iterator = root.item.find(name);
            if (iterator == root.item.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_object() && !iterator->is_array())
                return legion_fs_error("Item with name: " + name + " is not an object or array.");

            active_stack.emplace(name, *iterator);
        }
        else
        {
            auto& [curName, current, readIndex] = active_stack.top();

            if (current.is_array())
            {
                active_stack.emplace("item_" + std::to_string(readIndex), current[readIndex]);
                readIndex++;
            }
            else if (current.is_object())
            {
                auto iterator = current.find(name);
                if (iterator == current.end())
                    return legion_fs_error("Item with name: " + name + " is not in current object.");

                if (!iterator->is_object() && !iterator->is_array())
                    return legion_fs_error("Item with name: " + name + " is not an object or array.");

                active_stack.emplace(name, *iterator);
            }
        }
        return common::success;
    }

    common::result<void, fs_error> json::peak_ahead(const std::string& name)
    {
        if (active_stack.empty())
        {
            auto iterator = root.item.find(name);
            if (iterator == root.item.end())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!iterator->is_object() && !iterator->is_array())
                return legion_fs_error("Item with name: " + name + " is not an object or array.");

            peakObject = entry{ iterator.key(), *iterator };
        }
        else
        {
            auto& [curName, current, readIndex] = active_stack.top();

            if (current.is_array())
            {
                peakObject = entry{ "item_" + std::to_string(readIndex), current[readIndex] };
            }
            else if (current.is_object())
            {
                auto iterator = current.find(name);
                if (iterator == current.end())
                    return legion_fs_error("Item with name: " + name + " is not in current object.");

                if (!iterator->is_object() && !iterator->is_array())
                    return legion_fs_error("Item with name: " + name + " is not an object or array.");

                peakObject = entry{ iterator.key(), *iterator };
            }
        }
        isPeaking = true;
        return common::success;
    }

    void json::end_read()
    {
        if (active_stack.empty())
            return;

        active_stack.pop();
    }

    bool json::is_current_array()
    {
        return current_item().item.is_array();
    }

    bool json::is_current_object()
    {
        return current_item().item.is_object();
    }

    size_type json::current_item_size()
    {
        return current_item().item.size();
    }

    std::string json::get_key()
    {
        return current_item().key;
    }

    common::result<void, fs_error> json::write(fs::view& file)
    {
        return file.set(fs::basic_resource(root.item.dump(4)));
    }

    common::result<void, fs_error> json::read(const fs::view& file)
    {
        auto result = file.get();
        if (!result.valid())
        {
            log::error(result.error().what());
            return result.error();
        }
        root.key = "root";
        root.item = nlohmann::ordered_json::parse(result->to_string());
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> json::read(const byte_vec& data)
    {
        root.key = "root";
        root.item = nlohmann::ordered_json::parse(data);
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> json::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        root.key = "root";
        root.item = nlohmann::ordered_json::parse(begin, end);
        root.currentReadIndex = 0;
        return common::success;
    }
}
