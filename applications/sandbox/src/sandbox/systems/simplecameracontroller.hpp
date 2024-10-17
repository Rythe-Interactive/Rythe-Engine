#pragma once
#include "gui_test.hpp"
#include <application/application.hpp>
#include <core/core.hpp>
#include <graphics/graphics.hpp>
// #include "../data/crosshair.hpp"

#ifdef RYTHE_AUDIO
	#include <audio/audio.hpp>
#endif

using namespace rythe;

struct player_move : public app::input_axis<player_move>
{
};
struct player_strive : public app::input_axis<player_strive>
{
};
struct player_fly : public app::input_axis<player_fly>
{
};
struct player_look_x : public app::input_axis<player_look_x>
{
};
struct player_look_y : public app::input_axis<player_look_y>
{
};
struct player_speedup : public app::input_axis<player_speedup>
{
};

struct exit_action : public app::input_action<exit_action>
{
};
struct restart_action : public app::input_action<restart_action>
{
};

struct fullscreen_action : public app::input_action<fullscreen_action>
{
};
struct escape_cursor_action : public app::input_action<escape_cursor_action>
{
};
struct vsync_action : public app::input_action<vsync_action>
{
};

class SimpleCameraController final : public System<SimpleCameraController>
{
public:
	ecs::entity camera;

	bool escaped = true;
	float movementspeed = 5.f;

	void setup()
	{
		// Crosshair::setScale(math::float2(1.f));
		// gfx::PostProcessingStage::addEffect<Crosshair>(-100);
#pragma region Input binding
		app::InputSystem::createBinding<player_move>(app::inputmap::method::W, 1.f);
		app::InputSystem::createBinding<player_move>(app::inputmap::method::S, -1.f);
		app::InputSystem::createBinding<player_strive>(app::inputmap::method::D, 1.f);
		app::InputSystem::createBinding<player_strive>(app::inputmap::method::A, -1.f);
		app::InputSystem::createBinding<player_fly>(app::inputmap::method::SPACE, 1.f);
		app::InputSystem::createBinding<player_fly>(app::inputmap::method::LEFT_SHIFT, -1.f);
		app::InputSystem::createBinding<player_look_x>(app::inputmap::method::MOUSE_X, 0.f);
		app::InputSystem::createBinding<player_look_y>(app::inputmap::method::MOUSE_Y, 0.f);
		app::InputSystem::createBinding<player_speedup>(app::inputmap::method::LEFT_CONTROL, 3.f);
		app::InputSystem::createBinding<player_speedup>(app::inputmap::method::LEFT_ALT, -3.f);

		app::InputSystem::createBinding<exit_action>(app::inputmap::method::ESCAPE);
		app::InputSystem::createBinding<restart_action>(app::inputmap::method::F10);
		app::InputSystem::createBinding<fullscreen_action>(app::inputmap::method::F11);
		app::InputSystem::createBinding<escape_cursor_action>(app::inputmap::method::MOUSE_RIGHT);
		app::InputSystem::createBinding<vsync_action>(app::inputmap::method::F1);

		bindToEvent<player_move, &SimpleCameraController::onPlayerMove>();
		bindToEvent<player_strive, &SimpleCameraController::onPlayerStrive>();
		bindToEvent<player_fly, &SimpleCameraController::onPlayerFly>();
		bindToEvent<player_look_x, &SimpleCameraController::onPlayerLookX>();
		bindToEvent<player_look_y, &SimpleCameraController::onPlayerLookY>();
		bindToEvent<exit_action, &SimpleCameraController::onExit>();
		bindToEvent<restart_action, &SimpleCameraController::onRestart>();
		bindToEvent<player_speedup, &SimpleCameraController::onSpeedUp>();
		bindToEvent<fullscreen_action, &SimpleCameraController::onFullscreen>();
		bindToEvent<escape_cursor_action, &SimpleCameraController::onEscapeCursor>();
		bindToEvent<vsync_action, &SimpleCameraController::onVSYNCSwap>();

#pragma endregion

		app::window& window = ecs::world.get_component<app::window>();
		window.enableCursor(true);
		window.show();

		{
			app::context_guard guard(window);
			/*Crosshair::setTexture(gfx::TextureCache::create_texture("assets://textures/crosshair.png"_view, {
		gfx::texture_type::two_dimensional, channel_format::eight_bit, gfx::texture_format::rgba,
		gfx::texture_components::rgba, true, true, gfx::texture_mipmap::linear, gfx::texture_mipmap::linear,
		gfx::texture_wrap::edge_clamp, gfx::texture_wrap::edge_clamp, gfx::texture_wrap::edge_clamp }));*/
			setupCameraEntity();
		}

		createProcess<&SimpleCameraController::onGetCamera>("Update", 0.5f);
	}

	void onGetCamera(rsl::span)
	{
		static ecs::filter<gfx::camera> query{};

		if (query.size())
		{
			camera = query[0];
		}
	}

	void setupCameraEntity()
	{
		auto skyboxMat = rendering::MaterialCache::create_material("skybox", "assets://shaders/skybox.shs"_view);
		skyboxMat.set_param(SV_SKYBOX, TextureCache::create_texture("morning islands", fs::view("assets://textures/HDRI/morning_islands.jpg")));
		ecs::world.add_component(gfx::skybox_renderer{skyboxMat});

		auto groundplane = createEntity("Ground Plane");
		auto groundmat = rendering::MaterialCache::create_material("floor", "assets://shaders/groundplane.shs"_view);
		groundmat.set_param("floorTile", rendering::TextureCache::create_texture("floorTile", "engine://resources/default/tile.png"_view));
		groundplane.add_component(gfx::mesh_renderer{groundmat, rendering::ModelCache::create_model("floor", "assets://models/plane.obj"_view)});
		groundplane.add_component<transform>();

		camera = createEntity("Camera");
		camera.add_component<transform>(position(0.f, 3.f, -30.f), rotation::lookat(math::float3::zero, math::float3::forward), scale());

#ifdef RYTHE_AUDIO
		camera.add_component<audio::audio_listener>();
#endif

		rendering::camera cam;
		cam.set_projection(60.f, 0.001f, 1000.f);
		camera.add_component<gfx::camera>(cam);
	}

#pragma region input stuff
	void onRestart(restart_action& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (action.released())
			this_engine::restart();
	}

	void onExit(exit_action& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (action.released())
			raiseEvent<events::exit>();
	}

	void onSpeedUp(player_speedup& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		movementspeed = 5.f + action.value;
	}

	void onFullscreen(fullscreen_action& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (action.released())
		{
			app::WindowSystem::requestFullscreenToggle(ecs::world_entity_id, math::int2(100, 100), math::int2(1360, 768));
		}
	}

	void onEscapeCursor(escape_cursor_action& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (action.released() && !escaped)
		{
			app::window& window = ecs::world.get_component<app::window>();
			escaped = true;
			window.enableCursor(true);
		}
		else if (action.pressed() && escaped)
		{
			app::window& window = ecs::world.get_component<app::window>();
			escaped = false;
			window.enableCursor(false);
		}
		GuiTestSystem::CaptureKeyboard(!escaped);
	}

	void onVSYNCSwap(vsync_action& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (action.released())
		{
			app::window& window = ecs::world.get_component<app::window>();
			window.setSwapInterval(window.swapInterval() ? 0 : 1);
			log::debug("set swap interval to {}", window.swapInterval());
		}
	}

	void onPlayerMove(player_move& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (escaped)
			return;

		position& pos = camera.get_component<position>();
		rotation& rot = camera.get_component<rotation>();
		math::float3 move = math::toMat3(rot) * math::float3(0.f, 0.f, 1.f);
		pos += math::normalize(move * math::float3(1, 0, 1)) * action.value * action.input_delta * movementspeed;
	}

	void onPlayerStrive(player_strive& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (escaped)
			return;

		position& pos = camera.get_component<position>();
		rotation& rot = camera.get_component<rotation>();
		math::float3 move = math::toMat3(rot) * math::float3(1.f, 0.f, 0.f);
		pos += math::normalize(move * math::float3(1, 0, 1)) * action.value * action.input_delta * movementspeed;
	}

	void onPlayerFly(player_fly& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (escaped)
			return;

		position& pos = camera.get_component<position>();
		pos += (math::float3(0.f, action.value * action.input_delta * movementspeed, 0.f));
	}

	void onPlayerLookX(player_look_x& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (escaped)
			return;

		rotation& rot = camera.get_component<rotation>();
		rot = math::angleAxis(action.value * action.input_delta * 500.f, math::float3::up) * rot;
	}

	void onPlayerLookY(player_look_y& action)
	{
		if (GuiTestSystem::isEditingText)
			return;

		if (escaped)
			return;

		rotation& rot = camera.get_component<rotation>();
		math::float3x3 rotMat = math::toMat3(rot);
		math::float3 right = rotMat * math::float3::right;
		math::float3 fwd = math::normalize(math::cross(right, math::float3::up));
		math::float3 up = rotMat * math::float3::up;
		float angle = math::orientedAngle(fwd, up, right);

		angle += action.value * action.input_delta * 500.f;

		if (angle > -0.001f)
			angle = -0.001f;
		if (angle < -(math::pi<float>() - 0.001f))
			angle = -(math::pi<float>() - 0.001f);

		up = math::float3x3(math::axisAngleMatrix(right, angle)) * fwd;
		fwd = math::cross(right, up);
		rot = (rotation)math::conjugate(math::toQuat(math::lookAt(math::float3::zero, fwd, up)));
	}
#pragma endregion
};
