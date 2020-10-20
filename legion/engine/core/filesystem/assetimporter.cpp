#include <core/filesystem/assetimporter.hpp>

namespace legion::core::filesystem
{
    sparse_map<id_type, detail::resource_converter_base*> AssetImporter::m_converters;
}
