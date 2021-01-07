#pragma once
#include <variant>
#include <core/core.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/rendering.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>


#include "../data/animation.hpp"


namespace ext
{
    using namespace legion;

    namespace detail {

        class AnimationSequencer : public imgui::sequencer::SequenceInterface
        {
        public:

            //////////////////////////////////////////////////////
            ///INTERFACE
            ///
            int GetFrameMin() const override;

            int GetFrameMax() const override;

            int GetItemCount() const override;

            int GetItemTypeCount() const override;
            const char* GetItemTypeName(int type) const override;

            const char* GetItemLabel(int index) const override;

            void Get(int index, int** start, int** end, int* type,
                unsigned* color) override;

            void Add(int type) override;

            void Del(int index) override;

            void Duplicate(int index) override;
            ///
            //////////////////////////////////////////////////////

            /**
             * @brief sets the animation to work with
             */
            void SetAnimation(animation* anim);

            /**
             * @brief gets the modified animation
             */
            animation GetAnimation() const;


            /**
             * @brief gets a particular datapoint at a certain index
             */
            std::variant<position, rotation, scale>& GetDataPointAt(int index);
        private:

            const char* make_label(std::variant<position, rotation, scale> v) const
            {
                return std::holds_alternative<position>(v) ? "Position" : std::holds_alternative<rotation>(v) ? "Rotation" : "Scale";
            }

            struct InternalAnimationData
            {
                bool running, looping;
                float ra;
                float pa;
                float sa;
                index_type ri;
                index_type pi;
                index_type si;

            } m_savedAnimationData;

            int m_frameMax = 100;
            int m_frameMin = 0;
            struct SequencerData
            {
                std::vector<std::tuple<int, int, std::variant<position, rotation, scale>>> data;
            }m_currentAnimation;

        };
    }

    class AnimationEditor : public System<AnimationEditor>
    {
        ecs::entity_handle cubeEntity;

        void setup() override
        {
            using namespace legion::filesystem::literals;
            app::window window = m_ecs->world.get_component_handle<app::window>().read();

            rendering::model_handle cubeModel;
            rendering::material_handle vertexColorMaterial;

            {
                application::context_guard guard(window);

                cubeModel = rendering::ModelCache::create_model("cube - Animator", "assets://models/cube.obj"_view);
                vertexColorMaterial = rendering::MaterialCache::create_material("color shader - Animator", "assets://shaders/texture.shs"_view);
            }


            cubeEntity = createEntity();

            cubeEntity.add_components<transform>(position(), rotation(), scale());
            cubeEntity.add_components<rendering::mesh_renderable>(mesh_filter(cubeModel.get_mesh()), rendering::mesh_renderer(vertexColorMaterial));

            ext::animation anim;
            anim.looping = true;
            anim.running = true;
            filesystem::basic_resource res = fs::view("assets://test.anim").get().except([](auto err)
                {
                    return filesystem::basic_resource("{}");
                });
            anim = res.to<animation>();


            cubeEntity.add_component<animation>(anim);

            rendering::ImGuiStage::addGuiRender<AnimationEditor, &AnimationEditor::onGUI>(this);
        }


        int selectedEntry = 0;
        int firstFrame = 0;
        detail::AnimationSequencer sequencer;

        char filenamebuffer[512]{ 'a','s','s','e','t','s',':','\\','\\','t','e','s','t','.','a','n','i','m','\0' };

        void onGUI(application::window&, rendering::camera& cam, const rendering::camera::camera_input& cInput, time::span)
        {
            //TODO (algorythmix) anim here needs to come from the selected object, the problem ofc is that
            //TODO (cont.)       the concept of a "selected" object is not defined as such 

            auto anim = cubeEntity.read_component<animation>();
            using namespace imgui;
            static bool animatorHasControl = false;


            base::Begin("Animator");

            if (base::Button("Toggle Animation"))
            {
                anim.running = !anim.running;
            }

            base::SameLine();
            base::InputText("Location", filenamebuffer, 512);
            base::SameLine();
            if (base::Button("Save"))
            {
                fs::view f(filenamebuffer);
                f.set(filesystem::to_resource(anim));
            }
            base::Checkbox("Pause & Edit", &animatorHasControl);

            sequencer.SetAnimation(&anim);


            Sequencer(&sequencer, nullptr, nullptr, &selectedEntry, &firstFrame,
                sequencer::SEQUENCER_EDIT_STARTEND |
                sequencer::SEQUENCER_ADD |
                sequencer::SEQUENCER_DEL |
                sequencer::SEQUENCER_COPYPASTE |
                sequencer::SEQUENCER_CHANGE_FRAME);



            base::End();
            if (selectedEntry != -1)
            {
                auto& dp = sequencer.GetDataPointAt(selectedEntry);

                if (std::holds_alternative<position>(dp))
                {
                    base::Begin(" - Animator - Edit Position");
                    position& pos = std::get<position>(dp);
                    float v[3]{ pos.x,pos.y,pos.z };
                    base::InputFloat3("Position", v);
                    pos.x = v[0];
                    pos.y = v[1];
                    pos.z = v[2];
                    if (animatorHasControl)
                    {
                        math::mat4 model = math::compose(cubeEntity.read_component<scale>(), cubeEntity.read_component<rotation>(), pos);
                        ImGuiIO& io = ImGui::GetIO();

                        float aspect = io.DisplaySize.x / io.DisplaySize.y;
                        math::mat4 projection = projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);
                        gizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
                        gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection), gizmo::OPERATION::TRANSLATE, gizmo::MODE::LOCAL, value_ptr(model));

                        math::vec3 dummyv;
                        math::quat dummyq;
                        math::decompose(model, dummyv, dummyq, pos);

                    }
                    base::End();
                    if (animatorHasControl)
                    {
                        cubeEntity.get_component_handle<position>().write(pos);
                    }
                }
                if (std::holds_alternative<rotation>(dp))
                {
                    base::Begin(" - Animator - Edit Rotation");
                    rotation& rot = std::get<rotation>(dp);

                    auto euler = math::eulerAngles(rot);

                    float v[3]{ euler.x,euler.y,euler.z };
                    base::InputFloat3("Euler", v);


                    euler.x = v[0];
                    euler.y = v[1];
                    euler.z = v[2];

                    if (animatorHasControl)
                    {
                        math::mat4 model = math::compose(cubeEntity.read_component<scale>(), math::quat(euler), cubeEntity.read_component<position>());
                        ImGuiIO& io = ImGui::GetIO();

                        float aspect = io.DisplaySize.x / io.DisplaySize.y;
                        math::mat4 projection = projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);;
                        gizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
                        gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection), gizmo::OPERATION::ROTATE, gizmo::MODE::LOCAL, value_ptr(model));
                        math::vec3 nothing;

                        math::decompose(model, nothing, rot, nothing);
                    }


                    base::End();
                    if (animatorHasControl)
                    {
                        cubeEntity.get_component_handle<rotation>().write(rot);
                    }
                }
                if (std::holds_alternative<scale>(dp))
                {
                    base::Begin(" - Animator - Edit Scale");
                    scale& s = std::get<scale>(dp);
                    float v[3]{ s.x,s.y,s.z };
                    base::InputFloat3("Scale", v);
                    s.x = v[0];
                    s.y = v[1];
                    s.z = v[2];
                    if (animatorHasControl)
                    {
                        math::mat4 model = math::compose(s, cubeEntity.read_component<rotation>(), cubeEntity.read_component<position>());
                        ImGuiIO& io = ImGui::GetIO();

                        float aspect = io.DisplaySize.x / io.DisplaySize.y;
                        math::mat4 projection = projection = math::perspective(math::deg2rad(cam.fov * aspect), aspect, cam.nearz, cam.farz);
                        gizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
                        gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection), gizmo::OPERATION::SCALE, gizmo::MODE::LOCAL, value_ptr(model));

                        math::vec3 dummyv;
                        math::quat dummyq;
                        math::decompose(model, s, dummyq, dummyv);

                    }
                    base::End();
                    if (animatorHasControl)
                    {
                        cubeEntity.get_component_handle<scale>().write(s);
                    }
                }
            }



            anim = sequencer.GetAnimation();
            if (animatorHasControl)
            {
                anim.running = false;
            }
            cubeEntity.get_component_handle<animation>().write(anim);
        }
    };


}
