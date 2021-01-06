#pragma once
#include <core/core.hpp>
#include <audio/audio.hpp>
#include <application/application.hpp>
#include <rendering/rendering.hpp>
#include <tinygltf/json.hpp>

using namespace legion;

struct player_move : public app::input_axis<player_move> {};
struct player_strive : public app::input_axis<player_strive> {};
struct player_fly : public app::input_axis<player_fly> {};
struct player_look_x : public app::input_axis<player_look_x> {};
struct player_look_y : public app::input_axis<player_look_y> {};
struct player_speedup : public app::input_axis<player_speedup> {};

struct exit_action : public app::input_action<exit_action> {};

struct fullscreen_action : public app::input_action<fullscreen_action> {};
struct escape_cursor_action : public app::input_action<escape_cursor_action> {};
struct vsync_action : public app::input_action<vsync_action> {};

struct record_action : public app::input_action<record_action> {};
struct playback_action : public app::input_action<playback_action> {};


class SimpleCameraController final : public System<SimpleCameraController>
{
public:
    ecs::entity_handle camera;
    ecs::entity_handle skybox;
    ecs::entity_handle groundplane;

    bool m_loopAnimation = false;
    bool escaped = true;
    float movementspeed = 5.f;

    std::vector<std::pair<uint64_t, std::pair<position, rotation>>>
        m_animationTrack;
    index_type m_idx = std::numeric_limits<index_type>::max();


    position m_previousPosition;
    rotation m_previousRotation;
    float m_accumulator;


    virtual void setup()
    {
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
        app::InputSystem::createBinding<fullscreen_action>(app::inputmap::method::F11);
        app::InputSystem::createBinding<escape_cursor_action>(app::inputmap::method::MOUSE_RIGHT);
        app::InputSystem::createBinding<vsync_action>(app::inputmap::method::F1);
        app::InputSystem::createBinding<record_action>(app::inputmap::method::R);
        app::InputSystem::createBinding<playback_action>(app::inputmap::method::F9);

        bindToEvent<player_move, &SimpleCameraController::onPlayerMove>();
        bindToEvent<player_strive, &SimpleCameraController::onPlayerStrive>();
        bindToEvent<player_fly, &SimpleCameraController::onPlayerFly>();
        bindToEvent<player_look_x, &SimpleCameraController::onPlayerLookX>();
        bindToEvent<player_look_y, &SimpleCameraController::onPlayerLookY>();
        bindToEvent<exit_action, &SimpleCameraController::onExit>();
        bindToEvent<player_speedup, &SimpleCameraController::onSpeedUp>();
        bindToEvent<fullscreen_action, &SimpleCameraController::onFullscreen>();
        bindToEvent<escape_cursor_action, &SimpleCameraController::onEscapeCursor>();
        bindToEvent<vsync_action, &SimpleCameraController::onVSYNCSwap>();
        bindToEvent<record_action, &SimpleCameraController::onRecord>();
        bindToEvent<playback_action, &SimpleCameraController::onPlayBack>();
        createProcess<&SimpleCameraController::onUpdate>("Update");

#pragma endregion

        app::window window = world.read_component<app::window>();
        window.enableCursor(true);
        window.show();

        {
            app::context_guard guard(window);
            setupCameraEntity();
        }

        /*
        ext::animation animation;

        animation.looping = true;
        animation.running = true;
        animation.position_key_frames.emplace_back(5, position(1, 1, 1));
        animation.position_key_frames.emplace_back(2, position(2, 2, 2));
        animation.position_key_frames.emplace_back(4, position(2, 1, 1));
        animation.rotation_key_frames.emplace_back(3, rotation(math::quat(math::vec3(0, 0, 1))));
        animation.rotation_key_frames.emplace_back(3, rotation(math::quat(math::vec3(1, 0, 1))));
        animation.rotation_key_frames.emplace_back(3, rotation(math::quat(math::vec3(0, 1, 0))));
        animation.rotation_key_frames.emplace_back(2, rotation(math::quat(math::vec3(0, 1, 1))));
        animation.scale_key_frames.emplace_back(4, scale(1, 2, 1));
        animation.scale_key_frames.emplace_back(4, scale(2, 1, 3));

        camera.add_component<ext::animation>(animation);
        */
    }

    void setupCameraEntity()
    {
        skybox = createEntity();
        auto skyboxMat = rendering::MaterialCache::create_material("skybox", "assets://shaders/skybox.shs"_view);
        skyboxMat.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
        skybox.add_components<rendering::mesh_renderable>(mesh_filter(rendering::ModelCache::create_model("uvsphere", "assets://models/uvsphere.obj"_view).get_mesh()), rendering::mesh_renderer(skyboxMat));
        skybox.add_components<transform>(position(), rotation(), scale(1000.f));

        groundplane = createEntity();
        auto groundmat = rendering::MaterialCache::create_material("floor", "assets://shaders/groundplane.shs"_view);
        groundmat.set_param("floorTile", rendering::TextureCache::create_texture("floorTile", "engine://resources/default/tile.png"_view));
        groundplane.add_component<rendering::mesh_renderer>({ groundmat, rendering::ModelCache::create_model("floor", "assets://models/plane.obj"_view) });
        groundplane.add_components<transform>();
        groundplane.write_component(scale(250.f));
        camera = createEntity();
        camera.add_components<transform>(position(0.f, 3.f, 0.f), rotation::lookat(math::vec3::zero, math::vec3::forward), scale());
        camera.add_component<audio::audio_listener>();

        rendering::camera cam;
        cam.set_projection(22.5f, 0.001f, 1000.f);
        camera.add_component<rendering::camera>(cam);
    }

#pragma region input stuff

    //raphael was here
    void onRecord(record_action* action)
    {
        if (action->released())
        {
            static std::chrono::high_resolution_clock::time_point previous;
            auto now = std::chrono::high_resolution_clock::now();
            position pos = camera.get_component_handle<position>().read();
            rotation rot = camera.get_component_handle<rotation>().read();

            using json = nlohmann::json;

            auto file = fs::view("assets://camera.anim");

            std::string contents = file.get().except([](auto err)
                {
                    return filesystem::basic_resource(R"({ "animation":[] })");
                }).to_string();


                json j = json::parse(contents);
                log::debug("{}", j.dump());

                if (j.at("animation").empty())
                {
                    log::info("no running camera animation! making a new one");
                    json entry;
                    entry.emplace("position", json({ {"x",pos.x},{"y",pos.y},{"z",pos.z} }));
                    entry.emplace("rotation", json({ {"x",rot.x},{"y",rot.y},{"z",rot.z},{"w",rot.w} }));
                    j["animation"] += entry;
                }
                else
                {
                    auto last_frame = j.at("animation").back();

                    json entry;
                    last_frame.emplace("duration", (now - previous).count());
                    entry.emplace("position", json({ {"x",pos.x},{"y",pos.y},{"z",pos.z} }));
                    entry.emplace("rotation", json({ {"x",rot.x},{"y",rot.y},{"z",rot.z},{"w",rot.w} }));
                    j["animation"] += entry;

                }

                previous = now;

                file.set(filesystem::basic_resource(j.dump()));
        }
    }

    void onPlayBack(playback_action* action)
    {
        if (action->pressed())
        {
            using json = nlohmann::json;

            auto file = fs::view("assets://camera.anim");

            std::string contents = file.get().except([](auto err)
                {
                    return filesystem::basic_resource(R"({ "animation":[] })");
                }).to_string();


                json j = json::parse(contents);

                if (j.at("animation").empty())
                {
                    log::warn("Camera animation file was empty, skipping!");
                    return;
                }

                m_animationTrack.clear();

                for (auto& entry : j.at("animation"))
                {
                    log::debug("{}", entry.dump());
                    log::debug("{}", entry.at("position").dump());

                    position p;
                    p.x = static_cast<float>(entry["position"]["x"]);
                    p.y = static_cast<float>(entry["position"]["y"]);
                    p.z = static_cast<float>(entry["position"]["z"]);

                    rotation r;
                    r.w = static_cast<float>(entry["rotation"]["w"]);
                    r.x = static_cast<float>(entry["rotation"]["x"]);
                    r.y = static_cast<float>(entry["rotation"]["y"]);
                    r.z = static_cast<float>(entry["rotation"]["z"]);

                    if (entry.find("duration") != entry.end())
                    {
                        m_animationTrack.emplace_back(
                            std::make_pair(static_cast<uint64_t>(entry["duration"]), std::make_pair(p, r)));
                    }
                    else
                    {
                        m_animationTrack.emplace_back(std::make_pair(0ULL, std::make_pair(p, r)));
                    }
                }


                m_idx = 0ULL;
                m_previousPosition = camera.get_component_handle<position>().read();
                m_previousRotation = camera.get_component_handle<rotation>().read();
        }
    }

    void onUpdate(time::span dt)
    {
        if (m_idx != std::numeric_limits<index_type>::max()) {
            if (m_idx < m_animationTrack.size())
            {
                escaped = true;
                m_accumulator += dt.nanoseconds() / static_cast<float>(m_animationTrack[m_idx].first);

                rotation newRotation = m_animationTrack[m_idx].second.second;
                position newPosition = m_animationTrack[m_idx].second.first;

                camera.get_component_handle<position>().write(math::lerp(m_previousPosition, newPosition, m_accumulator));
                camera.get_component_handle<rotation>().write(math::slerp(m_previousRotation, newRotation, m_accumulator));
                if (m_accumulator > 1.0f)
                {
                    m_accumulator = 0.0f;
                    m_idx++;
                    m_previousPosition = newPosition;
                    m_previousRotation = newRotation;

                    if (m_idx > m_animationTrack.size())
                    {
                        if (m_loopAnimation)
                        {
                            m_idx = 0;
                        }
                        else
                        {
                            m_idx = m_idx = std::numeric_limits<index_type>::max();
                        }
                    }
                }
            }
        }
    }


    //


    void onExit(exit_action* action)
    {
        if (action->released())
            raiseEvent<events::exit>();
    }

    void onSpeedUp(player_speedup* action)
    {
        movementspeed = 5.f + action->value;
    }

    void onFullscreen(fullscreen_action* action)
    {
        if (action->released())
        {
            app::WindowSystem::requestFullscreenToggle(world_entity_id, math::ivec2(100, 100), math::ivec2(1360, 768));
        }
    }

    void onEscapeCursor(escape_cursor_action* action)
    {
        if (action->released() && !escaped)
        {
            app::window window = m_ecs->world.get_component_handle<app::window>().read();
            escaped = true;
            window.enableCursor(true);
        }
        else if (action->pressed() && escaped)
        {
            app::window window = m_ecs->world.get_component_handle<app::window>().read();
            escaped = false;
            window.enableCursor(false);
        }
    }

    void onVSYNCSwap(vsync_action* action)
    {
        if (action->released())
        {
            auto handle = m_ecs->world.get_component_handle<app::window>();
            app::window window = handle.read();
            window.setSwapInterval(window.swapInterval() ? 0 : 1);
            log::debug("set swap interval to {}", window.swapInterval());
            handle.write(window);
        }
    }

    void onPlayerMove(player_move* action)
    {
        if (escaped)
            return;

        auto posH = camera.get_component_handle<position>();
        auto rot = camera.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(0.f, 0.f, 1.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * movementspeed;
        posH.fetch_add(move);

        auto pos = posH.read();
        skybox.write_component(pos);
        groundplane.write_component(position(pos.x, 0, pos.z));
    }

    void onPlayerStrive(player_strive* action)
    {
        if (escaped)
            return;

        auto posH = camera.get_component_handle<position>();
        auto rot = camera.get_component_handle<rotation>().read();
        math::vec3 move = math::toMat3(rot) * math::vec3(1.f, 0.f, 0.f);
        move = math::normalize(move * math::vec3(1, 0, 1)) * action->value * action->input_delta * movementspeed;
        posH.fetch_add(move);

        auto pos = posH.read();
        skybox.write_component(pos);
        groundplane.write_component(position(pos.x, 0, pos.z));
    }

    void onPlayerFly(player_fly* action)
    {
        if (escaped)
            return;

        auto posH = camera.get_component_handle<position>();
        posH.fetch_add(math::vec3(0.f, action->value * action->input_delta * movementspeed, 0.f));

        auto pos = posH.read();
        skybox.write_component(pos);
    }

    void onPlayerLookX(player_look_x* action)
    {
        if (escaped)
            return;

        auto rotH = camera.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value * action->input_delta * 500.f, math::vec3(0, 1, 0)));
        rotH.read_modify_write([](rotation& src)
            {
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                src = (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }

    void onPlayerLookY(player_look_y* action)
    {
        if (escaped)
            return;

        auto rotH = camera.get_component_handle<rotation>();
        rotH.fetch_multiply(math::angleAxis(action->value * action->input_delta * 500.f, math::vec3(1, 0, 0)));
        rotH.read_modify_write([](rotation& src)
            {
                math::vec3 fwd = math::toMat3(src) * math::vec3(0.f, 0.f, 1.f);
                if (fwd.y < -0.95f)
                    fwd.y = -0.95f;
                else if (fwd.y > 0.95f)
                    fwd.y = 0.95f;
                fwd = math::normalize(fwd);
                math::vec3 right = math::cross(fwd, math::vec3(0.f, 1.f, 0.f));
                src = (rotation)math::conjugate(math::toQuat(math::lookAt(math::vec3(0.f, 0.f, 0.f), fwd, math::cross(right, fwd))));
            });
    }
#pragma endregion

};
