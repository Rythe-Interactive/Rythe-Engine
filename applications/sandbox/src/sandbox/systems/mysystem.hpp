#pragma once

///
/// This system is not used in the project and just here for an example!!!
///

#include <core/core.hpp>

class MySystem final : public ryt::System<MySystem>
{
public:
	auto to_string(ryt::channel_format format)
	{
		switch (format)
		{
			case ryt::channel_format::eight_bit:
				return "8bit";
			case ryt::channel_format::sixteen_bit:
				return "16bit";
			case ryt::channel_format::float_hdr:
				return "hdr";
			default:
				return "unknown";
		}
	}

	auto to_string(ryt::image_components components)
	{
		switch (components)
		{
			case ryt::image_components::grey:
				return "grey";
			case ryt::image_components::grey_alpha:
				return "grey alpha";
			case ryt::image_components::rgb:
				return "rgb";
			case ryt::image_components::rgba:
				return "rgba";
			default:
				return "unknown";
		}
	}

	ryt::async::async_operation<ryt::common::result<ryt::assets::asset<ryt::mesh>>> asyncOp;

	void setup()
	{
		using namespace rythe;
		auto result = assets::load<image>(fs::view("engine://resources/default/albedo"));
		if (!result)
			log::error("{}", result.error());

		if (result.has_warnings())
			for (auto& warn : result.warnings())
				log::warn(warn);

		[[maybe_unused]] auto val = result.except([]([[maybe_unused]] exception& error)
		{ return assets::invalid_asset<image>; });

		if (result)
		{
			auto img = result.value();
			// log::debug("\nname: {}\nid: {}\nresolution: {}\nformat: {}\ncomponents: {}",
			//     img.name(), img.id(), img->resolution(), to_string(img->format()), to_string(img->components()));
		}

		auto objR = assets::load<mesh>(fs::view("assets://models/submeshtest.obj"));
		if (!objR)
			log::error("{}", objR.error());

		if (objR.has_warnings())
			for (auto& warn : objR.warnings())
				log::warn(warn);

		if (objR)
		{
			auto msh = objR.value();
			// log::debug("\nname: {}\nid: {}\nvertex count: {}\ntriangle count: {}\nmaterial count: {}",
			//     msh.name(), msh.id(), msh->vertices.size(), msh->indices.size() / 3, msh->materials.size());
		}

		auto glbR = assets::load<mesh>(fs::view("assets://models/submeshtest.glb"));
		if (!glbR)
			log::error("{}", glbR.error());

		if (glbR.has_warnings())
			for (auto& warn : glbR.warnings())
				log::warn(warn);

		if (glbR)
		{
			auto msh = glbR.value();
			// log::debug("\nname: {}\nid: {}\nvertex count: {}\ntriangle count: {}\nmaterial count: {}",
			//     msh.name(), msh.id(), msh->vertices.size(), msh->indices.size() / 3, msh->materials.size());
		}
	}

	void update([[maybe_unused]] rsl::span deltaTime)
	{
		using namespace rythe;

		static bool tested = false;

		if (tested)
			return;

		tested = true;

		rsl::stopwatch<> clock;
		asyncOp = assets::loadAsync<mesh>(fs::view("assets://models/wizardgnome.glb"));

		while (!asyncOp.is_done())
			log::info("waiting... {:.2f}%", asyncOp.progress() * 100.f);

		log::debug("waited {}s", clock.elapsed_time().seconds());

		auto& gltfR = asyncOp.get_result();

		if (!gltfR)
			log::error("{}", gltfR.error());

		if (gltfR.has_warnings())
			for (auto& warn : gltfR.warnings())
				log::warn(warn);

		if (gltfR)
		{
			auto msh = gltfR.value();
			log::debug("\nname: {}\nid: {}\nvertex count: {}\ntriangle count: {}\nmaterial count: {}", msh.name(), msh.id(), msh->vertices.size(), msh->indices.size() / 3, msh->materials.size());
		}

		queueJobs(1, []()
		{
			ryt::log::error("Sleep start");
			std::this_thread::sleep_for(std::chrono::minutes(1));
			ryt::log::error("Sleep end");
		});

		ryt::async::rw_spinlock lock;
		lock.lock();

		auto procA = queueJobs(2000, [&lock](rsl::id_type jobId)
		{
			ryt::async::readonly_guard guard(lock);
			ryt::log::info("\tjob A id [{}]", jobId);
		});

		auto procB = queueJobs(2000, [&lock](rsl::id_type jobId)
		{
			ryt::async::readonly_guard guard(lock);
			ryt::log::debug("\tjob B id [{}]", jobId);
		});


		auto procC = queueJobs(2000, [&lock](rsl::id_type jobId)
		{
			ryt::async::readonly_guard guard(lock);
			ryt::log::warn("\tjob C id [{}]", jobId);
		});

		lock.unlock();
		procC.wait();
		procB.wait();
		procA.wait();
	}
};
