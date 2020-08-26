#pragma once
#include <core/containers/containers.hpp>
#include <core/filesystem/resource.hpp>
#include <core/filesystem/view.hpp>

namespace args::core::filesystem
{
	struct resource_converter_base {};

	template<typename... Settings>
	struct resource_converter : public resource_converter_base
	{
		virtual common::result_decay_more<basic_resource, fs_error> load(const basic_resource& resource, Settings&&...) ARGS_PURE;
	};

	struct basic_resource_converter final : public resource_converter<>
	{
		virtual common::result_decay_more<basic_resource, fs_error> load(const basic_resource& resource) override { return common::result_decay_more<basic_resource, fs_error>(common::Ok(basic_resource(resource))); }
	};

	class AssetImporter
	{
	private:
		static sparse_map<id_type, std::unique_ptr<resource_converter_base>> m_converters;

	public:
		
		template<typename T>
		static void reportConverter(cstring extension) 
		{
			m_loadfuncs.emplace(nameHash(extension), new T);
		}

		template<typename T, typename... Settings>
		static common::result_decay_more<T, fs_error> tryLoad(const view& view, Settings&&... settings)
		{
			using common::Err, common::Ok;
			// decay overloads the operator of ok_type and operator== for valid_t.
			using decay = common::result_decay_more<T, fs_error>;

			if (!view.is_valid() || !view.file_info().is_file)
				return decay(Err(args_fs_error("requested asset load on view that isn't a file.")));

			auto result = view.get();
			if (result != common::valid)
				return decay(Err(result.get_error()));

			return decay(Ok(m_converters[nameHash(view.get_extension())]->load(result, std::forward<Settings>(settings)...).to<T>()));
		}
	};
}
