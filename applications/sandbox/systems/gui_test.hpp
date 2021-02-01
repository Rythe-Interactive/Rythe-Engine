#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>
#include <rendering/systems/renderer.hpp>

using namespace legion::rendering;

class GuiTestSystem : public System<GuiTestSystem>
{
    ecs::EntityQuery cameraQuery = createQuery<camera, transform>();
    ecs::EntityQuery entityQuery = createQuery<transform>();

    material_handle vertexColorMaterial;
    model_handle cubeModel;
    ecs::entity_handle cubeEntity;


    char guiTextBuffer[512]{ 0 };

    math::mat4 view = math::mat4(1.0f);
    math::mat4 projection = math::mat4(1.0f);
    math::mat4 model = math::mat4(1.0f);

    bool* showSave = nullptr;

    void setup() override
    {

        static_cast<DefaultPipeline*>(Renderer::getMainPipeline())->attachStage<ImGuiStage>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            application::context_guard guard(window);

            cubeModel = ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColorMaterial = MaterialCache::create_material("decal", "assets://shaders/decal.shs"_view);
            vertexColorMaterial.set_param(SV_ALBEDO, TextureCache::create_texture("engine://resources/default/albedo"_view));
            vertexColorMaterial.set_param(SV_NORMALHEIGHT, TextureCache::create_texture("engine://resources/default/normalHeight"_view));
            vertexColorMaterial.set_param(SV_MRDAO, TextureCache::create_texture("engine://resources/default/MRDAo"_view));
            vertexColorMaterial.set_param(SV_EMISSIVE, TextureCache::create_texture("engine://resources/default/emissive"_view));
            vertexColorMaterial.set_param(SV_HEIGHTSCALE, 0.f);
            vertexColorMaterial.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
        }


        cubeEntity = createEntity();

        position pos(5.f, 0.f, 5.f);
        scale scal(3.f, 2.f, 3.f);
        rotation rot = angleAxis(math::quarter_pi<float>(), math::vec3::up);
        model = compose(scal, rot, pos);

        cubeEntity.add_components<transform>(pos, rot, scal);
        cubeEntity.add_components<mesh_renderable>(mesh_filter(cubeModel.get_mesh()), mesh_renderer(vertexColorMaterial));

        entityQuery.queryEntities();
        //gui code goes here
        ImGuiStage::addGuiRender<GuiTestSystem,&GuiTestSystem::onGUI>(this);
        createProcess<&GuiTestSystem::update>("Update");
    }


    bool offsetMode = false;
    float offset[3]{ 0 };



    // BuildTree creates a rudimentary Entity View, as entities do currently not have the ability to be named
    // This is going to display IDs, which isn't great but still gives a little insight
    // In how the Scene is currently structured, it will also try to show the names of the components of the
    // entities, which makes identifiying them easier
    //
    void BuildTree(ecs::entity_handle handle)
    {
        if (ImGui::TreeNode(reinterpret_cast<void*>(handle.get_id()), "%llu", handle.get_id())) {
            if (handle.has_component<hierarchy>())
            {
                auto hry = handle.read_component<hierarchy>();
                for (auto child : hry.children)
                {
                    BuildTree(child);
                }
            }

            if (ImGui::TreeNode("Components")) {
                for (id_type id : handle.component_composition())
                {
                    ImGui::Text(m_ecs->getFamilyName(id).c_str());
                }

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
    }


    void onGUI(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {

        scenemanagement::SceneManager::create_scene_entity();

        ImGuiIO& io = ImGui::GetIO();

        setProjectionAndView(io.DisplaySize.x / io.DisplaySize.y, cam, camInput);


        using namespace imgui;
        base::ShowDemoWindow();
        gizmo::SetOrthographic(false);
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("Save Scene"))
                {
                    std::string sceneName = "Main";
                    int count = scenemanagement::SceneManager::sceneNames.size();
                    sceneName += std::to_string(count);
                    std::string text = "Press here to save scene as:";
                    text += sceneName;
                    if (base::Button(text.c_str()))
                    {
                        scenemanagement::SceneManager::create_scene(sceneName);
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Load Scene"))
                {
                    auto sceneNames = scenemanagement::SceneManager::sceneNames;
                    for (auto& entry : sceneNames)
                    {
                        auto name = entry.second;
                        if (ImGui::MenuItem(name.c_str()))
                        {
                            scenemanagement::SceneManager::load_scene(name);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            base::Begin("Entities");
            BuildTree(m_ecs->world);
            base::End();

            ImGui::EndMainMenuBar();
        }

        //gizmo::BeginFrame();

       // base::End();

        gizmo::BeginFrame();
        gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
        base::Begin("Edit Cube Transform");
        //cannot render more than one gizmo at once (and animator also uses one)
        gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
        base::End();

        base::Begin("Edit Camera Transform");

        if (base::RadioButton("offset", offsetMode)) { offsetMode = true; }
        base::SameLine();
        if (base::RadioButton("set", !offsetMode)) { offsetMode = false; }

        base::InputFloat3("Modifier", offset);
        if (base::Button("Change"))
        {
            cameraQuery.queryEntities();
            auto handle = cameraQuery[0].get_component_handle<position>();

            if (offsetMode)
            {
                position p = handle.read();
                p.x += offset[0];
                p.y += offset[1];
                p.z += offset[2];
                handle.write(p);
            }
            else {
                position p;
                p.x = offset[0];
                p.y = offset[1];
                p.z = offset[2];
                handle.write(p);
            }
        }
        base::End();


        //gizmo::ViewManipulate(value_ptr(view), 1.0f, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), 0x10101010);
    }



    void update(time::span dt)
    {
        if (cubeEntity.valid() && !cubeEntity.has_component<camera>())
        {
            auto [mposh, mroth, mscaleh] = cubeEntity.get_component_handles<transform>();
            math::vec3 mpos = mposh.read();
            math::vec3 mscale = mscaleh.read();
            math::quat mrot = mroth.read();
            decompose(model, mscale, mrot, mpos);
            mposh.write(mpos);
            mroth.write(mrot);
            mscaleh.write(mscale);
        }
    }

    void setProjectionAndView(float aspect, const camera& cam, const camera::camera_input& camInput)
    {
        view = camInput.view;
        projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);
    }
};

