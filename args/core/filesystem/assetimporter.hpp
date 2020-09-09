#pragma once
#include <core/containers/containers.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/view.hpp>
#include <iostream>

#include "core/logging/logging.hpp"

namespace args::core::filesystem
{
    struct resource_converter_base
    {
    protected:
        virtual void dont_call() ARGS_PURE;
    };

    template<typename result, typename... Settings>
    struct resource_converter : public resource_converter_base
    {
        id_type result_type() { return typeHash<result>(); }

        virtual common::result_decay_more<basic_resource, fs_error> load(const basic_resource& resource, Settings&&...) ARGS_PURE;

    protected:
        virtual void dont_call() override {};
    };

    struct basic_resource_converter final : public resource_converter<basic_resource>
    {
        virtual common::result_decay_more<basic_resource, fs_error> load(const basic_resource& resource) override { return common::result_decay_more<basic_resource, fs_error>(common::Ok(basic_resource(resource))); }
    };

    class AssetImporter
    {
    private:
        struct ARGS_API data
        {
            static sparse_map<id_type, resource_converter_base*> m_converters;
        };

    public:      
        template<typename T>
        static void reportConverter(cstring extension)
        {
            data::m_converters.insert(nameHash(extension), new T());
        }

        template<typename T, typename... Settings>
        static common::result_decay_more<T, fs_error> tryLoad(const view& view, Settings&&... settings)
        {
            using common::Err, common::Ok;
            // decay overloads the operator of ok_type and operator== for valid_t.
            using decay = common::result_decay_more<T, fs_error>;

            if (!view.is_valid() || !view.file_info().is_file)
                return decay(Err(args_fs_error("requested asset load on view that isn't a valid file.")));

            auto result = view.get();
            if (result != common::valid)
                return decay(Err(result.get_error()));

            resource_converter_base* base = data::m_converters[nameHash(view.get_extension())];

            std::string typeName = typeid(T).name();
            log::debug("{}", typeName);

            if constexpr (sizeof...(Settings) > 0)
            {
                std::string settingNames[sizeof...(Settings)] = { typeid(Settings).name()... };

                for (std::string name : settingNames)
                   log::debug("{}", name );
            }

            auto* converter = reinterpret_cast<resource_converter<T, Settings...>*>(base);

            if (typeHash<T>() == converter->result_type())
            {
                auto loadresult = converter->load(result, std::forward<Settings>(settings)...);
                if (loadresult == common::valid)
                    return decay(Ok(((basic_resource)loadresult).to<T>()));

                return decay(Err(loadresult.get_error()));
            }

            return decay(Err(args_fs_error("requested asset load on file that stores a different type of asset.")));
        }

    };
}
