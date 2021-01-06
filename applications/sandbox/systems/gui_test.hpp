#pragma once
#include <core/core.hpp>
#include <rendering/rendering.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>

using namespace legion::rendering;

class GuiTestSystem : public System<GuiTestSystem>
{
    ecs::EntityQuery cameraQuery = createQuery<camera, position, rotation, scale>();


    material_handle vertexColorMaterial;
    model_handle cubeModel;
    ecs::entity_handle cubeEntity;


    char guiTextBuffer[512]{ 0 };

    math::mat4 view = math::mat4(1.0f);
    math::mat4 projection = math::mat4(1.0f);
    math::mat4 model = math::mat4(1.0f);


    void setup() override
    {

        static_cast<DefaultPipeline*>(Renderer::getMainPipeline())->attachStage<ImGuiStage>();

        app::window window = m_ecs->world.get_component_handle<app::window>().read();

        {
            application::context_guard guard(window);

            cubeModel = ModelCache::create_model("cube", "assets://models/cube.obj"_view);
            vertexColorMaterial = MaterialCache::create_material("color shader", "assets://shaders/texture.shs"_view);
        }


        cubeEntity = createEntity();

        cubeEntity.add_components<transform>(position(), rotation(), scale());
        cubeEntity.add_components<mesh_renderable>(mesh_filter(cubeModel.get_mesh()), mesh_renderer(vertexColorMaterial));


        //gui code goes here
        ImGuiStage::addGuiRender<GuiTestSystem,&GuiTestSystem::onGUI>(this);
        createProcess<&GuiTestSystem::update>("Update");
    }


    bool offsetMode = false;
    float offset[3]{0};

    void onGUI(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        setProjectionAndView(io.DisplaySize.x/io.DisplaySize.y, cam, camInput);


        using namespace imgui;
        base::ShowDemoWindow();
        gizmo::SetOrthographic(false);
        gizmo::BeginFrame();
        base::Begin("Edit Cube Transform");
        gizmo::EditTransform(value_ptr(view), value_ptr(projection), value_ptr(model), true);
        base::End();

        base::Begin("Edit Camera Transform");

        if(base::RadioButton("offset",offsetMode)) {offsetMode = true;}
        base::SameLine();
        if(base::RadioButton("set",  !offsetMode)) {offsetMode = false;}

        base::InputFloat3("Modifier",offset);
        if(base::Button("Change"))
        {
            cameraQuery.queryEntities();
            auto handle = cameraQuery[0].get_component_handle<position>();

            if(offsetMode)
            {
                position p = handle.read();
                p.x += offset[0];
                p.y += offset[1];
                p.z += offset[2];
                handle.write(p);    
            } else {
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

    void setProjectionAndView(float aspect, const camera& cam, const camera::camera_input& camInput)
    {
        view = camInput.view;
        projection = math::perspective(math::deg2rad(cam.fov*aspect),aspect,cam.nearz,cam.farz);
    }
};

