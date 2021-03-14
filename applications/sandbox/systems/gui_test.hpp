#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>
#include <rendering/systems/renderer.hpp>

using namespace legion;
using namespace legion::rendering;
using namespace legion::core::filesystem::literals;
using namespace legion::core::scenemanagement;

class GuiTestSystem : public System<GuiTestSystem>
{
public:
    static bool isEditingText;

private:
    ecs::EntityQuery cameraQuery = createQuery<camera, transform>();
    ecs::entity_handle selected = invalid_id;

    math::mat4 view = math::mat4(1.0f);
    math::mat4 projection = math::mat4(1.0f);
    math::mat4 model = math::mat4(1.0f);

    void setup() override
    {
        static_cast<DefaultPipeline*>(Renderer::getMainPipeline())->attachStage<ImGuiStage>();

        //gui code goes here
        ImGuiStage::addGuiRender<GuiTestSystem, &GuiTestSystem::onGUI>(this);

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        model_handle cubeModel;
        material_handle decalMaterial;

        {
            application::context_guard guard(window);

            cubeModel = ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            decalMaterial = MaterialCache::create_material("decal", "assets://shaders/decal.shs"_view);
            decalMaterial.set_param(SV_ALBEDO, TextureCache::create_texture("engine://resources/default/albedo"_view));
            decalMaterial.set_param(SV_NORMALHEIGHT, TextureCache::create_texture("engine://resources/default/normalHeight"_view));
            decalMaterial.set_param(SV_MRDAO, TextureCache::create_texture("engine://resources/default/MRDAo"_view));
            decalMaterial.set_param(SV_EMISSIVE, TextureCache::create_texture("engine://resources/default/emissive"_view));
            decalMaterial.set_param(SV_HEIGHTSCALE, 0.f);
            decalMaterial.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
        }

        auto decalEntity = createEntity();
        selected = decalEntity;
        hierarchy hry;
        hry.name = "Decal";
        decalEntity.add_component(hry);

        position pos(5.f, 0.f, 5.f);
        scale scal(3.f, 2.f, 3.f);
        rotation rot = angleAxis(math::quarter_pi<float>(), math::vec3::up);
        model = compose(scal, rot, pos);

        decalEntity.add_components<transform>(pos, rot, scal);
        decalEntity.add_components<mesh_renderable>(mesh_filter(cubeModel.get_mesh()), mesh_renderer(decalMaterial));
    }

    // BuildTree creates a rudimentary Entity View, as entities do currently not have the ability to be named
    // This is going to display IDs, which isn't great but still gives a little insight
    // In how the Scene is currently structured, it will also try to show the names of the components of the
    // entities, which makes identifying them easier
    //
    void BuildTree(ecs::entity_handle handle)
    {
        static bool isEditingName = false;
        static char buffer[512];

        std::string nodeName;

        if (handle == world)
            nodeName = "World";
        else if (handle.has_component<camera>())
            nodeName = "Camera";
        else if (handle.has_component<hierarchy>())
        {
            auto hry = handle.read_component<hierarchy>();
            if (hry.name.empty())
                nodeName = std::to_string(handle.get_id());
            else
                nodeName = hry.name;
        }
        else
            nodeName = std::to_string(handle.get_id());

        if (ImGui::TreeNode(reinterpret_cast<void*>(handle.get_id()), "%s", nodeName.c_str()))
        {
            if (ImGui::IsItemClicked())
            {
                selected = handle;
            }

            if (handle.has_component<hierarchy>())
            {
                auto hry = handle.read_component<hierarchy>();

                if (hry.name.empty())
                {
                    if (handle == world)
                        hry.name = "World";
                    else if (selected.has_component<camera>())
                        hry.name = "Camera";
                    else
                        hry.name = "Entity " + std::to_string(handle.get_id());

                    handle.write_component(hry);

                    if (selected == handle && isEditingName)
                        strcpy(buffer, hry.name.data());
                }

                if (!handle.has_component<camera>())
                {
                    if (selected == handle && isEditingName)
                    {
                        if (ImGui::InputText(" ", buffer, 512, ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            hry.name = buffer;
                            handle.write_component(hry);
                            isEditingName = false;
                            isEditingText = false;
                        }
                        if (ImGui::IsItemClicked())
                        {
                            selected = handle;
                        }
                    }
                    else
                    {
                        ImGui::Text("");
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Change Name"))
                    {
                        if (selected == handle && isEditingName)
                        {
                            hry.name = buffer;
                            handle.write_component(hry);
                            isEditingName = false;
                            isEditingText = false;
                        }
                        else
                        {
                            if (isEditingName)
                            {
                                auto selHry = selected.read_component<hierarchy>();
                                selHry.name = buffer;
                                selected.write_component(selHry);
                            }
                            selected = handle;
                            isEditingName = true;
                            isEditingText = true;
                            strcpy(buffer, hry.name.data());
                        }
                    }
                }
                else
                    ImGui::Text("Camera");

                if (selected != handle)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Select Entity"))
                    {
                        if (isEditingName)
                        {
                            auto selHry = selected.read_component<hierarchy>();
                            selHry.name = buffer;
                            selected.write_component(selHry);
                        }
                        isEditingName = false;
                        isEditingText = false;
                        selected = handle;
                    }
                }

                ImGui::Separator();

                if (hry.children.size() && ImGui::TreeNode("Children"))
                {
                    if (ImGui::IsItemClicked())
                    {
                        selected = handle;
                    }
                    for (auto child : hry.children)
                    {
                        BuildTree(child);
                    }
                    ImGui::TreePop();
                }
                else if (ImGui::IsItemClicked())
                {
                    selected = handle;
                }

            }
            else
            {
                if (!handle.has_component<camera>())
                {
                    ImGui::Text("");
                    ImGui::SameLine();
                    if (ImGui::Button("Add Name"))
                    {
                        handle.add_component<hierarchy>();
                        if (isEditingName)
                        {
                            auto selHry = selected.read_component<hierarchy>();
                            selHry.name = buffer;
                            selected.write_component(selHry);
                        }
                        selected = handle;
                        isEditingName = true;
                        isEditingText = true;
                    }
                }
                else
                    ImGui::Text("Camera");

                if (selected != handle)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Select Entity"))
                    {
                        if (isEditingName)
                        {
                            auto selHry = selected.read_component<hierarchy>();
                            selHry.name = buffer;
                            selected.write_component(selHry);
                        }
                        isEditingName = false;
                        isEditingText = false;
                        selected = handle;
                    }
                }
                ImGui::Separator();
            }

            if (ImGui::TreeNode("Components")) {
                if (ImGui::IsItemClicked())
                {
                    selected = handle;
                }
                for (id_type id : handle.component_composition())
                {
                    ImGui::Text("%s", m_ecs->getFamilyName(id).c_str());
                    if (ImGui::IsItemClicked())
                    {
                        selected = handle;
                    }
                }

                ImGui::TreePop();
            }
            else
            {
                if (ImGui::IsItemClicked())
                {
                    selected = handle;
                }
            }
            ImGui::TreePop();
        }
    }

    void DisplayFileHandling()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("Save Scene"))
                {
                    if (!SceneManager::currentScene)
                    {
                        SceneManager::currentScene = SceneManager::create_scene();
                    }
                    auto sceneEntity = SceneManager::currentScene.entity;
                    std::string sceneName = sceneEntity.get_name();

                    std::string text = "Save scene as:";
                    text += sceneName;
                    if (ImGui::Button(text.c_str()))
                    {
                        SceneManager::create_scene(sceneName, sceneEntity);
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Load Scene"))
                {
                    for (auto& [id, name] : SceneManager::sceneNames)
                    {
                        if (id && ImGui::MenuItem(name.c_str()))
                        {
                            SceneManager::load_scene(name);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void onGUI(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        if (!SceneManager::currentScene)
            SceneManager::currentScene = SceneManager::create_scene();

        ImGuiIO& io = ImGui::GetIO();

        setProjectionAndView(io.DisplaySize.x / io.DisplaySize.y, cam, camInput);

        using namespace imgui;
        gizmo::SetOrthographic(false);

        DisplayFileHandling();

        std::string windowName = "Entities";

        if (selected)
        {
            windowName += "  (selected:";

            auto hry = selected.read_component<hierarchy>();

            if (hry.name.empty())
            {
                if (selected == world)
                    hry.name = "World";
                else if (selected.has_component<camera>())
                    hry.name = "Camera";
                else
                    hry.name = "Entity " + std::to_string(selected.get_id());
            }

            windowName += hry.name;

            windowName += ")";
        }

        windowName += "###";

        if (base::Begin(windowName.c_str()))
        {
            BuildTree(m_ecs->world);
        }
        base::End();

        static bool showGizmo = true;

        gizmo::BeginFrame();

        base::Begin("Edit Transform");
        if (selected)
        {
            if (selected.has_component<camera>())
            {
                ImGui::Text("Changing camera transform not allowed.");
            }
            else if (selected.has_component<position>() || selected.has_component<rotation>() || selected.has_component<scale>())
            {
                if (base::RadioButton("Show Gizmo", showGizmo))
                    showGizmo = !showGizmo;

                if (showGizmo)
                {
                    auto pos = selected.read_component<position>();
                    auto rot = selected.read_component<rotation>();
                    auto scal = selected.read_component<scale>();
                    model = compose(scal, rot, pos);
                    gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
                    decompose(model, scal, rot, pos);
                    selected.write_component<position>(pos);
                    selected.write_component<rotation>(rot);
                    selected.write_component<scale>(scal);
                }
            }
            else
            {
                ImGui::Text("Entity has no transform components.");
            }
        }
        else
        {
            ImGui::Text("Select an entity to edit its transform.");
        }
        base::End();
    }

    void setProjectionAndView(float aspect, const camera& cam, const camera::camera_input& camInput)
    {
        view = camInput.view;
        projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);
    }
};

