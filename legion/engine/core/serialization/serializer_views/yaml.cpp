#include <core/serialization/serializer_views/yaml.hpp>

namespace legion::core::serialization
{
    yaml::entry& yaml::current_item()
    {
        if (isPeaking)
            return peakObject;
        return active_stack.top();
    }

    void yaml::serialize_int(const std::string& name, int serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    void yaml::serialize_float(const std::string& name, float serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    void yaml::serialize_double(const std::string& name, double serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    void yaml::serialize_bool(const std::string& name, bool serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    void yaml::serialize_string(const std::string& name, const std::string& serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    void yaml::serialize_id_type(const std::string& name, id_type serializable)
    {
        auto& [key, item, rdIdx] = current_item();
        if (item.IsMap())
            item[name] = serializable;
        else if (item.IsSequence())
            item.push_back(serializable);
    }

    common::result<int, fs_error> yaml::deserialize_int(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not an integer number.");

            return node.as<int>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not an integer number.");

            readIndex++;
            return item.as<int>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    common::result<float, fs_error> yaml::deserialize_float(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not a floating point number.");

            return node.as<float>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not a floating point number.");

            readIndex++;
            return item.as<float>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    common::result<double, fs_error> yaml::deserialize_double(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not a floating point number.");

            return node.as<double>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not a floating point number.");

            readIndex++;
            return item.as<double>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    common::result<bool, fs_error> yaml::deserialize_bool(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not a boolean.");

            return node.as<bool>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not a boolean.");

            readIndex++;
            return item.as<bool>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    common::result<std::string, fs_error> yaml::deserialize_string(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not a string.");

            return node.as<std::string>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not a string.");

            readIndex++;
            return item.as<std::string>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    common::result<id_type, fs_error> yaml::deserialize_id_type(const std::string& name)
    {
        auto& [currentName, current, readIndex] = current_item();
        if (current.IsMap())
        {
            auto node = current[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsScalar())
                return legion_fs_error("Item with name: " + name + " is not an integer number.");

            return node.as<id_type>();
        }
        else if (current.IsSequence())
        {
            if (readIndex >= current.size())
                return legion_fs_error("Tried reading yaml beyond the size of the array.");
            auto item = current[readIndex];

            if (!item.IsScalar())
                return legion_fs_error("Item #" + std::to_string(readIndex) + " in yaml array " + currentName + " is not an integer number.");

            readIndex++;
            return item.as<id_type>();
        }
        return legion_fs_error("Current yaml object is invalid.");
    }

    void yaml::start_object()
    {
        active_stack.emplace("item_" + std::to_string(current_item().item.size()), YAML::Node(YAML::NodeType::Map));
    }

    void yaml::start_object(const std::string& name)
    {
        active_stack.emplace(name, YAML::Node(YAML::NodeType::Map));
    }

    void yaml::end_object()
    {
        if (active_stack.empty())
            return;

        auto& [name, object, readIndex] = current_item();

        if (!object.IsMap())
            return;

        if (active_stack.size() == 1)
        {
            root.item[name] = object;
            active_stack.pop();
        }
        else
        {
            entry cpy{ name, object, readIndex };
            active_stack.pop();
            auto& [nxtName, next, nxtRdIdx] = current_item();

            if (next.IsSequence())
                next.push_back(cpy.item);
            else if (next.IsMap())
                next[cpy.key] = cpy.item;
        }
    }

    void yaml::start_container(const std::string& name)
    {
        active_stack.emplace(name, YAML::Node(YAML::NodeType::Sequence));
    }

    void yaml::end_container()
    {
        if (active_stack.empty())
            return;

        auto& [name, arr, readIndex] = current_item();

        if (!arr.IsSequence())
            return;

        if (active_stack.size() == 1)
        {
            root.item[name] = arr;
            active_stack.pop();
        }
        else
        {
            entry cpy{ name, arr, readIndex };
            active_stack.pop();
            auto& [nxtName, next, nxtRdIdx] = current_item();

            if (next.IsSequence())
                next.push_back(arr);
            else if (next.IsMap())
                next[cpy.key] = cpy.item;
        }
    }

    common::result<void, fs_error> yaml::start_read(const std::string& name)
    {
        isPeaking = false;
        if (active_stack.empty())
        {
            auto node = root.item[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsMap() && !node.IsSequence())
                return legion_fs_error("Item with name: " + name + " is not an object or array.");

            active_stack.emplace(name, node);
        }
        else
        {
            auto& [curName, current, readIndex] = active_stack.top();

            if (current.IsSequence())
            {
                active_stack.emplace("item_" + std::to_string(readIndex), current[readIndex]);
                readIndex++;
            }
            else if (current.IsMap())
            {
                auto node = current[name];
                if (node.IsNull())
                    return legion_fs_error("Item with name: " + name + " is not in current object.");

                if (!node.IsMap() && !node.IsSequence())
                    return legion_fs_error("Item with name: " + name + " is not an object or array.");

                active_stack.emplace(name, node);
            }
        }
        return common::success;
    }

    common::result<void, fs_error> yaml::peak_ahead(const std::string& name)
    {
        if (active_stack.empty())
        {
            auto node = root.item[name];
            if (node.IsNull())
                return legion_fs_error("Item with name: " + name + " is not in current object.");

            if (!node.IsMap() && !node.IsSequence())
                return legion_fs_error("Item with name: " + name + " is not an object or array.");

            peakObject = entry{ name, node };
        }
        else
        {
            auto& [curName, current, readIndex] = active_stack.top();

            if (current.IsSequence())
            {
                peakObject = entry{ "item_" + std::to_string(readIndex), current[readIndex] };
            }
            else if (current.IsMap())
            {
                auto node = current[name];
                if (node.IsNull())
                    return legion_fs_error("Item with name: " + name + " is not in current object.");

                if (!node.IsMap() && !node.IsSequence())
                    return legion_fs_error("Item with name: " + name + " is not an object or array.");

                peakObject = entry{ name, node };
            }
        }
        isPeaking = true;
        return common::success;
    }

    void yaml::end_read()
    {
        if (active_stack.empty())
            return;

        active_stack.pop();
    }

    bool yaml::is_current_array()
    {
        return current_item().item.IsSequence();
    }

    bool yaml::is_current_object()
    {
        return current_item().item.IsSequence();
    }

    size_type yaml::current_item_size()
    {
        return current_item().item.size();
    }

    std::string yaml::get_key()
    {
        return current_item().key;
    }

    common::result<void, fs_error> yaml::write(fs::view& file)
    {
        YAML::Emitter output;
        output.SetIndent(4);
        output << root.item;
        return file.set(fs::basic_resource(output.c_str()));
    }

    common::result<void, fs_error> yaml::read(const fs::view& file)
    {
        auto result = file.get();
        if (!result.valid())
        {
            log::error(result.error().what());
            return result.error();
        }
        root.key = "root";
        root.item = YAML::Load(result->to_string());
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> yaml::read(const byte_vec& data)
    {
        root.key = "root";
        std::string str(data.begin(), data.end());
        root.item = YAML::Load(str);
        root.currentReadIndex = 0;
        return common::success;
    }

    common::result<void, fs_error> yaml::read(byte_vec::iterator begin, byte_vec::iterator end)
    {
        root.key = "root";
        std::string str(begin, end);
        root.item = YAML::Load(str);
        root.currentReadIndex = 0;
        return common::success;
    }

}
