#pragma once
#include <core/containers/containers.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/view.hpp>
#include <iostream>

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

    class ARGS_API AssetImporter
    {
    private:
        static sparse_map<id_type, std::unique_ptr<resource_converter_base>> m_converters;

        static void addConverter(id_type hash, resource_converter_base* ptr);
        static resource_converter_base* getConverter(id_type has);

    public:
        void foo();
      
        template<typename T>
        static void reportConverter(cstring extension)
        {
            addConverter(nameHash(extension), new T);
        }

        template<typename T, typename... Settings>
        static common::result_decay_more<T, fs_error> tryLoad(const view& view, Settings&&... settings)
        {
            AssetImporter a;
            a.foo();

            using common::Err, common::Ok;
            // decay overloads the operator of ok_type and operator== for valid_t.
            using decay = common::result_decay_more<T, fs_error>;

            if (!view.is_valid() || !view.file_info().is_file)
                return decay(Err(args_fs_error("requested asset load on view that isn't a valid file.")));

            auto result = view.get();
            if (result != common::valid)
                return decay(Err(result.get_error()));

            auto* converter = dynamic_cast<resource_converter<T, Settings...>*>(getConverter(nameHash(view.get_extension())));

            if (!converter)
                return decay(Err(args_fs_error("requested asset load with wrong settings types.")));

            if (typeHash<T>() == converter->result_type())
            {
                common::result_decay_more<basic_resource, fs_error> result = converter->load(result, std::forward<Settings>(settings)...);
                if (result == common::valid)
                    return decay(Ok(((basic_resource)result).to<T>()));

                return decay(Err(result.get_error()));
            }

            return decay(Err(args_fs_error("requested asset load on file that stores a different type of asset.")));
        }

    };
}
