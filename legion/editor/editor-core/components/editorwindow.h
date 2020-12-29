#pragma once
#include <core/core.hpp>
#include <vector>

namespace legion::editor
{
	struct editor_surface
	{
		// insert rml ui data.

		// parent or child decides docking.

		// Turn off docking behavior in order to allow dragging.
		bool floating;
	};

	struct editor_window
	{
		std::vector<ecs::component_handle<editor_surface>> surfaces; // All surfaces attached to this window.
	};
}
