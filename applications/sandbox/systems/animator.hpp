#pragma once
#include <core/engine/system.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>
#include <rendering/util/gui.hpp>

#include "../data/animation.hpp"

namespace ext
{
    using namespace legion;

    class Animator : public System<Animator>
    {
        ecs::EntityQuery query = createQuery<animation, position, rotation, scale>();
        ecs::entity_handle cubeEntity;

        class AnimationSequencer : public imgui::sequencer::SequenceInterface
        {
        public:


            int GetFrameMin() const override
            {
                return m_frameMin;
            }
            int GetFrameMax() const override
            {
                return m_frameMax;
            }
            int GetItemCount() const override
            {
                return static_cast<int>(m_currentAnimation.data.size());
            }

            int GetItemTypeCount() const override { return 3; }
            const char* GetItemTypeName(int type) const override
            {
                return type == 0 ? "Position" : type == 1 ? "Rotation" : "Scale";
            }
            const char* GetItemLabel(int index)const  override
            {
                static char tmps[512];
                sprintf_s(tmps, "[%02d] %s", index, make_label(std::get<2>(m_currentAnimation.data[index])));
                return tmps;
            }

            void Get(int index, int** start, int** end, int* type, unsigned* color) override
            {
                //index of the rotation key frames


                if (start)  *start = &std::get<0>(m_currentAnimation.data[index]);
                if (end)    *end = &std::get<1>(m_currentAnimation.data[index]);
                if (type)
                {
                    if (std::holds_alternative<position>(std::get<2>(m_currentAnimation.data[index])))
                    {
                        if (color) *color = 0xA3BE8CFF;
                        *type = 0;
                    }
                    if (std::holds_alternative<rotation>(std::get<2>(m_currentAnimation.data[index])))
                    {
                        if (color) *color = 0xd08770FF;
                        *type = 1;
                    }
                    if (std::holds_alternative<scale>(std::get<2>(m_currentAnimation.data[index])))
                    {
                        if (color) *color = 0xb48eadFF;
                        *type = 2;
                    }
                }

            }
            animation GetAnimation() const
            {
                animation anim;
                /*
                std::sort(m_currentAnimation.data.begin(), m_currentAnimation.data.end(), [](const auto& a, const auto& b)
                {
                    return std::get<0>(a) < std::get<0>(b);
                });
                */
                for (const auto& [start, end, variant] : m_currentAnimation.data)
                {
                    if (std::holds_alternative<position>(variant))
                    {
                        anim.position_key_frames.emplace_back(end - start, std::get<position>(variant));
                    }
                    if (std::holds_alternative<rotation>(variant))
                    {
                        anim.rotation_key_frames.emplace_back(end - start, std::get<rotation>(variant));
                    }
                    if (std::holds_alternative<scale>(variant))
                    {
                        anim.scale_key_frames.emplace_back(end - start, std::get<scale>(variant));
                    }
                }

                anim.p_accumulator = m_savedAnimationData.pa;
                anim.r_accumulator = m_savedAnimationData.ra;
                anim.s_accumulator = m_savedAnimationData.sa;
                anim.p_index = m_savedAnimationData.pi;
                anim.r_index = m_savedAnimationData.ri;
                anim.s_index = m_savedAnimationData.si;
                anim.looping = m_savedAnimationData.looping;
                anim.running = m_savedAnimationData.running;

                return anim;
            }
            void Add(int type) override
            {
                if (type == 0)
                    m_currentAnimation.data.emplace_back(0, 0, position());
                if (type == 1)
                    m_currentAnimation.data.emplace_back(0, 0, rotation());
                if (type == 2)
                    m_currentAnimation.data.emplace_back(0, 0, scale());

            }
            void Del(int index) override
            {
                m_currentAnimation.data.erase(m_currentAnimation.data.begin() + index);
            }
            void Duplicate(int index) override
            {
                auto& [start, end, anim_data] = m_currentAnimation.data[index];
                m_currentAnimation.data.emplace_back(end, 10, anim_data);

            }

            void SetAnimation(animation* anim)
            {
                m_currentAnimation.data.clear();



                if (anim)
                {
                    m_savedAnimationData.pa = anim->p_accumulator;
                    m_savedAnimationData.ra = anim->r_accumulator;
                    m_savedAnimationData.sa = anim->s_accumulator;
                    m_savedAnimationData.pi = anim->p_index;
                    m_savedAnimationData.ri = anim->r_index;
                    m_savedAnimationData.si = anim->s_index;
                    m_savedAnimationData.looping = anim->looping;
                    m_savedAnimationData.running = anim->running;

                    int start = 0;
                    for (auto& entry : anim->position_key_frames)
                    {
                        m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                        start += entry.first + 1;
                    }

                    start = 0;
                    for (auto& entry : anim->rotation_key_frames)
                    {
                        m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                        start += entry.first + 1;
                    }

                    start = 0;
                    for (auto& entry : anim->scale_key_frames)
                    {
                        m_currentAnimation.data.emplace_back(start, entry.first + start, entry.second);
                        start += entry.first + 1;
                    }
                }
            }

            std::variant<position, rotation, scale>& GetDataPointAt(int index)
            {
                return std::get<2>(m_currentAnimation.data.at(index));
            }


        private:

            const char* make_label(std::variant<position, rotation, scale> v) const
            {
                return std::holds_alternative<position>(v) ? "Position" : std::holds_alternative<rotation>(v) ? "Rotation" : "Scale";
            }

            struct readAnimationData
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

        void setup() override
        {
            using namespace legion::filesystem::literals;
            app::window window = m_ecs->world.get_component_handle<app::window>().read();

            model_handle cubeModel;
            material_handle vertexColorMaterial;

            {
                application::context_guard guard(window);

                cubeModel = ModelCache::create_model("cube - Animator", "assets://models/cube.obj"_view);
                vertexColorMaterial = MaterialCache::create_material("color shader - Animator", "assets://shaders/texture.shs"_view);
            }


            cubeEntity = createEntity();

            cubeEntity.add_components<transform>(position(), rotation(), scale());
            cubeEntity.add_components<mesh_renderable>(mesh_filter(cubeModel.get_mesh()), mesh_renderer(vertexColorMaterial));

            ext::animation anim;
            anim.looping = true;
            anim.running = true;
            deserialize_animation(fs::view("assets://test.anim"), anim);
            cubeEntity.add_component<animation>(anim);

            rendering::ImGuiStage::addGuiRender<Animator, &Animator::onGUI>(this);
            createProcess<&Animator::onUpdate>("Update");
        }

        void onUpdate(time::span delta)
        {
            query.queryEntities();

            for (ecs::entity_handle ent : query)
            {
                animation anim = ent.read_component<animation>();

                //Animation disabled, early out
                if (!anim.running) continue;


                auto [pos, rot, sc] = ent.get_component_handles<transform>();

                if (!anim.position_key_frames.empty()) {
                    //POSITION ANIMATION
                    if (!anim.looping && anim.p_index == 0)
                    {
                        pos.write(anim.position_key_frames[0].second);
                        anim.p_index++;

                    }

                    const float duration = static_cast<float>(anim.position_key_frames[anim.p_index].first);

                    anim.p_accumulator += delta.seconds() / duration;

                    if (anim.p_accumulator >= 1.0f)
                    {
                        anim.p_accumulator = .0f;
                        anim.p_index++;
                        if (anim.p_index >= anim.position_key_frames.size())
                        {
                            if (anim.looping)
                                anim.p_index = 0;
                            else
                            {
                                pos.write(anim.position_key_frames[anim.position_key_frames.size() - 1].second);
                                anim.running = false;
                                break;

                            }
                        }
                    }
                    if (!anim.running)
                        continue;

                    index_type prev_index = anim.p_index == 0 ? anim.position_key_frames.size() - 1 : anim.p_index - 1;
                    auto newPos = math::lerp(anim.position_key_frames[prev_index].second, anim.position_key_frames[anim.p_index].second, anim.p_accumulator);

                    pos.write(newPos);

                }

                if (!anim.rotation_key_frames.empty()) {
                    // ROTATION ANIMATION
                    if (!anim.looping && anim.r_index == 0)
                    {
                        rot.write(anim.rotation_key_frames[0].second);
                        anim.r_index++;

                    }

                    const float duration = static_cast<float>(anim.rotation_key_frames[anim.r_index].first);

                    anim.r_accumulator += delta.seconds() / duration;

                    if (anim.r_accumulator >= 1.0f)
                    {
                        anim.r_accumulator = .0f;
                        anim.r_index++;
                        if (anim.r_index >= anim.rotation_key_frames.size())
                        {
                            if (anim.looping)
                                anim.r_index = 0;
                            else
                            {
                                rot.write(anim.rotation_key_frames[anim.rotation_key_frames.size() - 1].second);
                                anim.running = false;
                                continue;

                            }
                        }
                    }

                    index_type prev_index = anim.r_index == 0 ? anim.rotation_key_frames.size() - 1 : anim.r_index - 1;
                    auto newRot = math::slerp(anim.rotation_key_frames[prev_index].second, anim.rotation_key_frames[anim.r_index].second, anim.r_accumulator);
                    rot.write(newRot);
                }

                if (!anim.scale_key_frames.empty()) {
                    //SCALE ANIMATION
                    if (!anim.looping && anim.s_index == 0)
                    {
                        sc.write(anim.scale_key_frames[0].second);
                        anim.s_index++;

                    }

                    const float duration = static_cast<float>(anim.scale_key_frames[anim.s_index].first);
                    anim.s_accumulator += delta.seconds() / duration;
                    log::debug("{}", anim.s_accumulator);

                    if (anim.s_accumulator >= 1.0f)
                    {
                        anim.s_accumulator = .0f;
                        anim.s_index++;
                        if (anim.s_index >= anim.scale_key_frames.size())
                        {
                            if (anim.looping)
                                anim.s_index = 0;
                            else
                            {
                                sc.write(anim.scale_key_frames[anim.scale_key_frames.size() - 1].second);
                                anim.running = false;
                                continue;

                            }
                        }
                    }

                    index_type prev_index = anim.s_index == 0 ? anim.scale_key_frames.size() - 1 : anim.s_index - 1;
                    auto newSc = math::lerp(anim.scale_key_frames[prev_index].second, anim.scale_key_frames[anim.s_index].second, anim.s_accumulator);
                    sc.write(newSc);
                }

                ent.get_component_handle<animation>().write(anim);
            }
        }


        int selectedEntry = 0;
        int firstFrame = 0;
        AnimationSequencer sequencer;

        char filenamebuffer[512]{ 'a','s','s','e','t','s',':','\\','\\','t','e','s','t','.','a','n','i','m','\0' };

        void onGUI(application::window&, camera& cam, const camera::camera_input& cInput, time::span)
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
                serialize_animation(f, anim);
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
                        math::decompose(model,dummyv,dummyq,pos);

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
                        math::decompose(model,s,dummyq,dummyv);

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
