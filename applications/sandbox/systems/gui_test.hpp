#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>
#include <rendering/systems/renderer.hpp>

using namespace legion::rendering;

class GuiTestSystem : public System<GuiTestSystem>
{
    ecs::EntityQuery cameraQuery = createQuery<rendering::camera, position, rotation, scale>();


    rendering::material_handle vertexColorMaterial;
    rendering::model_handle cubeModel;
    ecs::entity_handle cubeEntity;


    char guiTextBuffer[512]{ 0 };

    math::mat4 view = math::mat4(1.0f);
    math::mat4 projection = math::mat4(1.0f);
    math::mat4 model = math::mat4(1.0f);


    void setup() override
    {

        static_cast<rendering::DefaultPipeline*>(rendering::Renderer::getMainPipeline())->attachStage<rendering::ImGuiStage>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            application::context_guard guard(window);

            cubeModel = rendering::ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColorMaterial = rendering::MaterialCache::create_material("color shader", "assets://shaders/texture.shs"_view);
        }


        cubeEntity = createEntity();

        cubeEntity.add_components<transform>(position(), rotation(), scale());
        cubeEntity.add_components<rendering::mesh_renderable>(mesh_filter(cubeModel.get_mesh()), rendering::mesh_renderer(vertexColorMaterial));


        //gui code goes here
        rendering::ImGuiStage::addGuiRender<GuiTestSystem,&GuiTestSystem::onGUI>(this);
        createProcess<&GuiTestSystem::update>("Update");
    }

    void onGUI()
    {
        ImGuiIO& io = ImGui::GetIO();

        setProjectionAndView(io.DisplaySize.x/io.DisplaySize.y);


        using namespace imgui;
        base::ShowDemoWindow();
        gizmo::SetOrthographic(false);
        gizmo::BeginFrame();
        base::Begin("Hello World");
        gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
        base::End();

        base::Begin("Window");
        base::Text("Hello World!");


        base::End();


        //gizmo::ViewManipulate(value_ptr(view), 1.0f, ImVec2(io.DisplaySize.x - 128, 0), ImVec2(128, 128), 0x10101010);
    }



    void update(time::span dt)
    {

        if (cubeEntity.valid())
        {
            auto [mposh, mroth, mscaleh] = cubeEntity.get_component_handles<transform>();
            math::vec3 mpos, mscale;
            math::quat mrot;
            decompose(model, mscale, mrot, mpos);
            mposh.write(mpos);
            mroth.write(mrot);
            mscaleh.write(mscale);
        }

    }

    void setProjectionAndView(float aspect)
    {
        cameraQuery.queryEntities();
        ecs::entity_handle cam_ent = cameraQuery[0];
        auto [cposh, croth, cscaleh] = cam_ent.get_component_handles<transform>();

        math::mat4 temp(1.0f);
        math::compose(temp, cscaleh.read(), croth.read(), cposh.read());
        view = inverse(temp);

        const auto cam = cam_ent.get_component_handle<rendering::camera>().read();
        const float ratio = 16.0f/9.0f;
        projection = math::perspective(math::deg2rad(cam.fov*aspect),aspect,cam.nearz,cam.farz);
    }
};

