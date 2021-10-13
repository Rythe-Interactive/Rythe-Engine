#pragma once
#include <any>
#include <memory>

#include <core/containers/containers.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/view.hpp>
#include <core/logging/logging.hpp>

#include <Optick/optick.h>

/**
 * @file assetimporter.hpp
 */

namespace legion::core::filesystem
{
    namespace detail
    {
        /**@class resource_converter_base
         * @brief Common base struct of all resource converters.
         */
        struct resource_converter_base
        {
            virtual id_type result_type() LEGION_PURE;
        };

    }

    /**@class resource_converter
     * @brief Base struct all user created resource converters should inherit from.
     * @tparam result Result type that the converter converts to.
     * @tparam Settings... Any settings to be passed to the load function.
     */
    template<typename result, typename... Settings>
    struct resource_converter : public detail::resource_converter_base
    {

        virtual id_type result_type() override { return typeHash<result>(); }

        virtual common::result<result, fs_error> load_default(const basic_resource& resource) LEGION_PURE;
        virtual common::result<result, fs_error> load(const basic_resource& resource, Settings&&...) LEGION_PURE;

    };

    /**@class basic_resource_converter
     * @brief Converter from basic resource to basic resource (essentially does nothing and allows you to load files as binary or text form through the AssetImporter).
     */
    struct basic_resource_converter final : public resource_converter<basic_resource>
    {
        common::result<basic_resource, fs_error> load_default(const basic_resource& resource) override { return load(resource); }
        virtual common::result<basic_resource, fs_error> load(const basic_resource& resource) override { return resource; }
    };

    /**@class basic_converter
     * @brief Simple default converter that uses from_resource<T> to convert from binary to the item to import.
     * @ref legion::core::filesystem::from_resource
     */
    template<typename T>
    struct basic_converter final : public resource_converter<T>
    {
        virtual common::result<T, fs_error> load(const basic_resource& resource) { return from_resource<T>(resource); }
    };

    /**@class AssetImporter
     * @brief Static class that allows you to load any object type from a file as long as a converter exists for that combination.
     */
    class AssetImporter
    {
    private:
        static sparse_map<id_type, std::vector<std::unique_ptr<detail::resource_converter_base>>> m_converters;

    public:
        /**@brief Reports a converter type to the importer and allows converting from the given extension to the given object type.
         * @param extension File extension to which the converter belongs.
         * @tparam T Type of the converter.
         */
        template<typename T>
        static void reportConverter(cstring extension)
        {
            OPTICK_EVENT();
            m_converters[nameHash(extension)].push_back(std::make_unique<T>());
        }


        /**@brief Attempt to load an object from a file using the pre-reported converters.
         * @param view filesystem::view to the file to load.
         * @param settings... Settings to pass to the load function of the converter.
         * @tparam T Type of the object to try to load.
         * @return common::result<T, fs_error> Result containing either an error or the successfully loaded object.
         */
        template<typename T, typename... Settings>
        static common::result<T, fs_error> tryLoad(const view& view, Settings&&... settings)
        {
            OPTICK_EVENT();

            // Debug log the settings used for loading the files so that you can track down why something got loaded wrong if it did.
            if constexpr (sizeof...(settings) == 0)
                log::trace("Tried to load asset of type{}", nameOfType<T>());
            else if constexpr (sizeof...(settings) == 1)
                log::trace("Tried to load asset of type{} with settings of type:{}", nameOfType<T>(), (std::string(nameOfType<Settings>()) + ...));
            else
                log::trace("Tried to load asset of type{} with settings of types:{}", nameOfType<T>(), ((std::string(nameOfType<Settings>()) + ", ") + ...));

            // Check if the view is valid to load as a file.
            if (!view.is_valid() || !view.file_info().is_file)
                return legion_fs_error("requested asset load on view that isn't a valid file.");

            // Get data from file and check validity.
            auto result = view.get();
            if (result != common::valid)
                return result.error();

            for (auto& base : m_converters[nameHash(view.get_extension())])
            {
                // Do a safety check if the cast was valid before we call any functions on it.
                if (typeHash<T>() == base->result_type())
                {
                    // Retrieve the correct converter to use.
                    auto* converter = static_cast<resource_converter<T, Settings...>*>(base.get());

                    // Attempt the conversion and return the result.
                    return converter->load(result, std::forward<Settings>(settings)...);
                }
            }

            return legion_fs_error("requested asset load on file that stores a different type of asset.");
        }

    };
}
