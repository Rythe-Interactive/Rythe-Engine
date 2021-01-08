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
            vertexColorMaterial = MaterialCache::create_material("decal", "assets://shaders/decal.shs"_view);
            vertexColorMaterial.set_param(SV_ALBEDO, rendering::TextureCache::create_texture("engine://resources/default/albedo"_view));
            vertexColorMaterial.set_param(SV_NORMALHEIGHT, rendering::TextureCache::create_texture("engine://resources/default/normalHeight"_view));
            vertexColorMaterial.set_param(SV_MRDAO, rendering::TextureCache::create_texture("engine://resources/default/MRDAo"_view));
            vertexColorMaterial.set_param(SV_EMISSIVE, rendering::TextureCache::create_texture("engine://resources/default/emissive"_view));
            vertexColorMaterial.set_param(SV_HEIGHTSCALE, 0.f);
            vertexColorMaterial.set_param("skycolor", math::color(0.1f, 0.3f, 1.0f));
        }


        cubeEntity = createEntity();

        cubeEntity.add_components<transform>(position(), rotation(), scale());
        cubeEntity.add_components<mesh_renderable>(mesh_filter(cubeModel.get_mesh()), mesh_renderer(vertexColorMaterial));


        //gui code goes here
        ImGuiStage::addGuiRender<GuiTestSystem,&GuiTestSystem::onGUI>(this);
        createProcess<&GuiTestSystem::update>("Update");
    }

    void onGUI(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        ImGuiIO& io = ImGui::GetIO();

        setProjectionAndView(io.DisplaySize.x/io.DisplaySize.y, cam, camInput);


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

    void setProjectionAndView(float aspect, const camera& cam, const camera::camera_input& camInput)
    {
        view = camInput.view;
        projection = math::perspective(math::deg2rad(cam.fov*aspect),aspect,cam.nearz,cam.farz);
    }
};

