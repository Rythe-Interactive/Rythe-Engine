#include <core/filesystem/assetimporter.hpp>

namespace args::core::filesystem
{
    sparse_map<id_type, std::unique_ptr<resource_converter_base>> AssetImporter::m_converters;

    void AssetImporter::addConverter(id_type hash, resource_converter_base* ptr)
    {
        m_converters.emplace(hash, ptr);
    }

    resource_converter_base* AssetImporter::getConverter(id_type hash)
    {
        return m_converters[hash].get();
    }

    void AssetImporter::foo()
    {
        std::cout << m_converters.size() << std::endl;
    }

}
