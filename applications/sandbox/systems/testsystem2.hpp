#pragma once
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/filesystem/filesystem.hpp>
#include <rendering/data/material.hpp>
#include <core/logging/logging.hpp>
#include <imgui/ImGuizmo.h>
#include <rendering/debugrendering.hpp>

#include <rendering/components/renderable.hpp>


#include "pointcloud_particlesystem.hpp"
#include <rendering/components/particle_emitter.hpp>

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
                m_cubeent.add_components<transform>(position(),rotation(),scale(1));
                model = math::compose(scale(1.f),rotation(),position());
                m_cubeent.add_component<rendering::renderable>({cube,directionalLightMH});

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
            EditTransform(value_ptr(view),value_ptr(projection),value_ptr(model),true);


            if(ImGui::Button("Ey look at me!"))
            {
                log::debug("This is called! {}",buffer);
                
            }
            ImGui::InputText("Text",buffer,512);
            ImGui::End();
            //ImGuizmo::ViewManipulate(value_ptr(view), 1.0f, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), 0x10101010);
        };

    }
    char buffer[512]{0};
    math::mat4 view;
    math::mat4 projection;
    math::mat4 model = math::mat4(1.0f);
    void update(time::span dt)
    {
        ecs::entity_handle cam_ent = cameraQuery[0];
        auto [cposh,croth,cscaleh] =  cam_ent.get_component_handles<transform>();

        math::mat4 temp(1.0f);
        math::compose(temp,cscaleh.read(),croth.read(),cposh.read());
        view = inverse(temp);

        projection = cam_ent.get_component_handle<rendering::camera>().read().get_projection(16.0f/9.0f);

        if(m_cubeent.valid())
        {
            auto [mposh,mroth,mscaleh] = m_cubeent.get_component_handles<transform>();
            math::vec3 mpos,mscale,dummy;
            math::quat mrot;
            math::decompose(model,mscale,mrot,mpos);
          //  ImGuizmo::DecomposeMatrixToComponents(value_ptr(model), value_ptr(mpos), value_ptr(dummy), value_ptr(mscale));
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
