#include <core/filesystem/assetimporter.hpp>

namespace args::core::filesystem
{
    sparse_map<id_type, detail::resource_converter_base*> AssetImporter::data::m_converters;
}
