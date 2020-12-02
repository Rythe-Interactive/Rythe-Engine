#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <imgui/ImGuizmo.h>
#include <imgui/imnodes.h>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>


#include "pointcloud_particlesystem.hpp"
#include <rendering/components/particle_emitter.hpp>
#include <imgui/ImGuiFileBrowser.h>

using namespace legion;

void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix, bool editTransformDecomposition);

class TestSystem2 final : public System<TestSystem2>
{
public:
    TestSystem2()
    {
        log::filter(log::severity::debug);
        app::WindowSystem::requestWindow(world_entity_id, math::ivec2(1360, 768), "LEGION Engine", "Legion Icon", nullptr, nullptr, 1);
    }
    ecs::EntityQuery cameraQuery = createQuery<rendering::camera, position, rotation, scale>();
    ecs::entity_handle m_cubeent;
    std::vector<std::pair<int, int>> links;
    imgui_addons::ImGuiFileBrowser file_dialog;

    virtual void setup()
    {
        rendering::model_handle cube;
        rendering::material_handle flatGreen;
        rendering::material_handle vertexColor;
        rendering::material_handle directionalLightMH;

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            async::readwrite_guard guard(*window.lock);
            app::ContextHelper::makeContextCurrent(window);


            auto colorshader = rendering::ShaderCache::create_shader("color", "assets://shaders/color.shs"_view);
            directionalLightMH = rendering::MaterialCache::create_material("directional light", colorshader);
            directionalLightMH.set_param("color", math::color(1, 1, 0.8f));

            cube = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColor = rendering::MaterialCache::create_material("vertex color", "assets://shaders/vertexcolor.shs"_view);


            std::vector<math::vec3> positions{
                math::vec3(0,1.0f,0),
                math::vec3(0,1.25f,0),
                math::vec3(0,1.5f,0),
                math::vec3(0,1.75f,0),

                math::vec3(1,1.0f,0),
                math::vec3(1,1.25f,0),
                math::vec3(1,1.5f,0),
                math::vec3(1,1.75f,0),

                math::vec3(1,1.0f,1),
                math::vec3(1,1.25f,1),
                math::vec3(1,1.5f,1),
                math::vec3(1,1.75f,1),

                math::vec3(0,1.0f,1),
                math::vec3(0,1.25f,1),
                math::vec3(0,1.5f,1),
                math::vec3(0,1.75f,1)
            };
            pointCloudParameters params{
            params.startingSize = math::vec3(0.2f),
            params.particleMaterial = vertexColor,
                params.particleModel = cube
            };
            auto pointcloud = rendering::ParticleSystemCache::createParticleSystem<PointCloudParticleSystem>("point_cloud", params, positions);

#pragma region entities

            {
                auto ent = createEntity();
                ent.add_components<transform>(position(-5, 0.01f, 0), rotation(), scale(1));
                rendering::particle_emitter emitter = ent.add_component<rendering::particle_emitter>().read();
                emitter.particleSystemHandle = pointcloud;
                ent.get_component_handle<rendering::particle_emitter>().write(emitter);
            }

            {
                m_cubeent = createEntity();
                m_cubeent.add_components<transform>(position(), rotation(), scale(1));
                model = math::compose(scale(1.f), rotation(), position());
                m_cubeent.add_components<rendering::renderable>(cube.get_mesh(), rendering::mesh_renderer(directionalLightMH));

            }

#pragma endregion
            createProcess<&TestSystem2::update>("Update");


        }


        rendering::Renderer::receiveGui += [this]()
        {
            ImGui::ShowDemoWindow();

            ImGuiIO& io = ImGui::GetIO();
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::BeginFrame();
            ImGui::Begin("Hello World");
            EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);


            if (ImGui::Button("Ey look at me!"))
            {
                log::debug("This is called! {}", buffer);

            }
            ImGui::InputText("Text", buffer, 512);
            ImGui::End();
            ImGuizmo::ViewManipulate(value_ptr(view), 1.0f, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), 0x10101010);
            ImGui::Begin("node editor");
            imnodes::BeginNodeEditor();

            // elsewhere in the code...
            for (int i = 0; i < links.size(); ++i)
            {
                const std::pair<int, int>& p = links[i];
                // in this case, we just use the array index of the link
                // as the unique identifier
                imnodes::Link(i, p.first, p.second);
            }
            imnodes::BeginNode(1);
            imnodes::BeginNodeTitleBar();
            ImGui::TextUnformatted("output node");
            imnodes::EndNodeTitleBar();
            ImGui::Dummy({ 80,45 });
            const int output_attr_id = 2;
            imnodes::BeginOutputAttribute(output_attr_id);
            // in between Begin|EndAttribute calls, you can call ImGui
            // UI functions
            ImGui::Text("output pin");
            imnodes::EndOutputAttribute();
            imnodes::EndNode();

            imnodes::BeginNode(2);
            imnodes::BeginNodeTitleBar();
            ImGui::TextUnformatted("input node");
            imnodes::EndNodeTitleBar();
            ImGui::Dummy({ 80,45 });
            imnodes::BeginInputAttribute(3);
            // in between Begin|EndAttribute calls, you can call ImGui
            // UI functions
            ImGui::Text("input pin");
            imnodes::EndOutputAttribute();
            imnodes::EndNode();

            imnodes::EndNodeEditor();
            int start_attr, end_attr;
            if (imnodes::IsLinkCreated(&start_attr, &end_attr))
            {
                links.push_back(std::make_pair(start_attr, end_attr));
            }

            const int num_selected = imnodes::NumSelectedLinks();
            if (num_selected > 0 && ImGui::IsKeyReleased(GLFW_KEY_X))
            {
                static std::vector<int> selected_links;
                selected_links.resize(static_cast<size_t>(num_selected));
                imnodes::GetSelectedLinks(selected_links.data());
                for (auto& link : selected_links)
                {
                    links.erase(links.begin() + link);
                }
            }
            ImGui::End();
            static bool open = false;
            static bool save = false;
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Menu"))
                {
                    if (ImGui::MenuItem("Open", NULL))
                        open = true;
                    if (ImGui::MenuItem("Save", NULL))
                        save = true;

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (open)
                ImGui::OpenPopup("Open File");
            if (save)
                ImGui::OpenPopup("Save File");

            if (file_dialog.showFileDialog("Open File", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".rar,.zip,.7z"))
            {
                open = false;
                log::debug("{}", file_dialog.selected_fn);     // The name of the selected file or directory in case of Select Directory dialog mode
                log::debug("{}", file_dialog.selected_path);   // The absolute path to the selected file
            }
            if (file_dialog.showFileDialog("Save File", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".png,.jpg,.bmp"))
            {
                save = false;
                log::debug("{}", file_dialog.selected_fn);      // The name of the selected file or directory in case of Select Directory dialog mode
                log::debug("{}", file_dialog.selected_path);    // The absolute path to the selected file
                log::debug("{}", file_dialog.ext);              // Access ext separately (For SAVE mode)
               //Do writing of files based on extension here
            }
        };
    }
    char buffer[512]{ 0 };
    math::mat4 view;
    math::mat4 projection;
    math::mat4 model = math::mat4(1.0f);
    void update(time::span dt)
    {
        cameraQuery.queryEntities();
        ecs::entity_handle cam_ent = cameraQuery[0];
        auto [cposh, croth, cscaleh] = cam_ent.get_component_handles<transform>();

        math::mat4 temp(1.0f);
        math::compose(temp, cscaleh.read(), croth.read(), cposh.read());
        view = inverse(temp);

        projection = cam_ent.get_component_handle<rendering::camera>().read().get_projection(16.0f / 9.0f);

        if (m_cubeent.valid())
        {
            auto [mposh, mroth, mscaleh] = m_cubeent.get_component_handles<transform>();
            math::vec3 mpos, mscale, dummy;
            math::quat mrot;
            decompose(model, mscale, mrot, mpos);
            mposh.write(mpos);
            mroth.write(mrot);
            mscaleh.write(mscale);
        }

    }
};


void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    if (editTransformDecomposition)
    {
        if (ImGui::IsKeyPressed(90))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        ImGui::Checkbox("", &useSnap);
        ImGui::SameLine();

        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        }
        ImGui::Checkbox("Bound Sizing", &boundSizing);
        if (boundSizing)
        {
            ImGui::PushID(3);
            ImGui::Checkbox("", &boundSizingSnap);
            ImGui::SameLine();
            ImGui::InputFloat3("Snap", boundsSnap);
            ImGui::PopID();
        }
    }
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
}
