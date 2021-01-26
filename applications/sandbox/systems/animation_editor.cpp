#include "animation_editor.hpp"

namespace ext
{
    sparse_map<id_type, AnimationEditor::custom_render_gui_fn> AnimationEditor::m_guiRenderers;

    int detail::AnimationSequencer::GetFrameMin() const
    {
        //minimum frame in sequencer
        return m_frameMin;
    }

    int detail::AnimationSequencer::GetFrameMax() const
    {
        //maximum frame in sequencer
        return m_frameMax;
    }


    int detail::AnimationSequencer::GetItemCount() const
    {
        //amount of data points
        return static_cast<int>(m_currentAnimation.data.size());
    }

    int detail::AnimationSequencer::GetItemTypeCount() const
    {
        //amount of unique data types
        return 4;
    }

    const char* detail::AnimationSequencer::GetItemTypeName(int type) const
    {
        //name of data type
        return type == 0 ? "Position" : type == 1 ? "Rotation" : type == 2 ? "Scale" : "Event";
    }

    const char* detail::AnimationSequencer::GetItemLabel(int index) const
    {
        //label at the beginning of the sequencer
        static char tmps[512];
        sprintf_s(tmps, "[%02d] %s", index, make_label(std::get<2>(m_currentAnimation.data[index])));
        return tmps;
    }

    void detail::AnimationSequencer::Get(int index, int** start, int** end, int* type, unsigned* color)
    {
        //get data about individual datapoint
        if (start) *start = &std::get<0>(m_currentAnimation.data[index]);
        if (end) *end = &std::get<1>(m_currentAnimation.data[index]);

        //data unique to each dp
        if (std::holds_alternative<position>(std::get<2>(m_currentAnimation.data[index])))
        {
            if (color) *color = 0xA3BE8CFF;
            if (type) *type = 0;
        }
        if (std::holds_alternative<rotation>(std::get<2>(m_currentAnimation.data[index])))
        {
            if (color) *color = 0xD08770FF;
            if (type) *type = 1;
        }
        if (std::holds_alternative<scale>(std::get<2>(m_currentAnimation.data[index])))
        {
            if (color) *color = 0xB48EADFF;
            if (type) *type = 2;
        }
        if (std::holds_alternative<event_t>(std::get<2>(m_currentAnimation.data[index])))
        {
            if (color) *color = 0x88C0D0FF;
            if (type) *type = 3;
        }


    }

    animation detail::AnimationSequencer::GetAnimation() const
    {
        //recalculates animation from data-points

        animation anim;

        //reconstruct animation from individual data points
        for (const auto& [start, end, variant] : m_currentAnimation.data)
        {
            if (std::holds_alternative<position>(variant))
            {
                // since the sequencer data uses end  & start we need to calculate a new
                // duration from that
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
            if (std::holds_alternative<event_t>(variant))
            {
                auto& ev = std::get<event_t>(variant);
                anim.events.emplace_back(start, std::make_pair(ev.first, ev.second));
            }
        }

        //write internal animation data
        //TODO(algo-ryht-mix): some of this could be hooked up in the future
        //to show progress in the animation and to control if the animation is looping
        anim.p_accumulator = m_savedAnimationData.pa;
        anim.r_accumulator = m_savedAnimationData.ra;
        anim.s_accumulator = m_savedAnimationData.sa;
        anim.e_accumulator = m_savedAnimationData.ea;

        anim.p_index = m_savedAnimationData.pi;
        anim.r_index = m_savedAnimationData.ri;
        anim.s_index = m_savedAnimationData.si;
        anim.e_index = m_savedAnimationData.ei;
        anim.looping = m_savedAnimationData.looping;
        anim.running = m_savedAnimationData.running;

        return anim;
    }

    void detail::AnimationSequencer::Add(int type)
    {
        // depending on the type we add a new datapoint entry 
        if (type == 0)
            m_currentAnimation.data.emplace_back(0, 0, position());
        if (type == 1)
            m_currentAnimation.data.emplace_back(0, 0, rotation());
        if (type == 2)
            m_currentAnimation.data.emplace_back(0, 0, scale());
        if (type == 3)
            m_currentAnimation.data.emplace_back(0, 0, std::make_pair(std::make_shared<animation_event_base>(), void_animation_event::id));
    }

    void detail::AnimationSequencer::Del(int index)
    {
        // erase dp at index
        m_currentAnimation.data.erase(m_currentAnimation.data.begin() + index);
    }

    void detail::AnimationSequencer::Duplicate(int index)
    {
        //duplicates a dp
        //copy the data from the existing dp to the new one
        auto& [start, end, anim_data] = m_currentAnimation.data[index];
        m_currentAnimation.data.emplace_back(end, 10, anim_data);
    }

    void detail::AnimationSequencer::SetAnimation(animation* anim)
    {
        //remove all old animation data
        m_currentAnimation.data.clear();


        if (anim)
        {
            //set internal extra data
            m_savedAnimationData.pa = anim->p_accumulator;
            m_savedAnimationData.ra = anim->r_accumulator;
            m_savedAnimationData.sa = anim->s_accumulator;
            m_savedAnimationData.ea = anim->e_accumulator;
            m_savedAnimationData.pi = anim->p_index;
            m_savedAnimationData.ri = anim->r_index;
            m_savedAnimationData.si = anim->s_index;
            m_savedAnimationData.ei = anim->e_index;
            m_savedAnimationData.looping = anim->looping;
            m_savedAnimationData.running = anim->running;

            //iterate over keyframes and convert to dps
            float start = 0;
            for (auto& entry : anim->position_key_frames)
            {
                //here we need to calculate a start and an end, since the keyframes only have a duration
                m_currentAnimation.data.emplace_back(static_cast<int>(start), entry.first + start, entry.second);

                //we also add spacing otherwise the dps would overlap in the sequencer
                start += entry.first + 1.0f;
            }

            start = 0;
            for (auto& entry : anim->rotation_key_frames)
            {
                m_currentAnimation.data.emplace_back(static_cast<int>(start), entry.first + start, entry.second);
                start += entry.first + 1.0f;
            }

            start = 0;
            for (auto& entry : anim->scale_key_frames)
            {
                m_currentAnimation.data.emplace_back(static_cast<int>(start), entry.first + start, entry.second);
                start += entry.first + 1.0f;
            }
            for (auto& [frame, entry] : anim->events)
            {
                event_t ev = std::make_pair(entry.first, entry.second);
                m_currentAnimation.data.emplace_back(static_cast<int>(frame), static_cast<int>(frame + 1.0f), ev);

            }

        }
    }

    detail::AnimationSequencer::holder& detail::AnimationSequencer::GetDataPointAt(int index)
    {
        //return the variant that contains the data
        return std::get<2>(m_currentAnimation.data.at(index));
    }

    void AnimationEditor::onGUI(application::window&, rendering::camera& cam,
        const rendering::camera::camera_input& cInput, time::span)
    {
        //TODO (algorythmix) anim here needs to come from the selected object, the problem of course 
        //TODO (cont.)       is that the concept of a "selected" object is not defined as such 
        auto anim = m_cubeEntity.read_component<animation>();

        //we are going to use a lot of imgui stuff here!
        using namespace imgui;


        // create a new Window for the Animator
        base::Begin("Animator");

        // this button enables & disables the currently selected animation 
        if (base::Button("Toggle Animation"))
        {
            anim.running = !anim.running;
        }

        // this section allows you to save the animation to disk under a different file name
        base::SameLine();
        base::InputText("Location", m_filenameBuffer, 512);
        base::SameLine();
        if (base::Button("Save"))
        {
            fs::view f(m_filenameBuffer);
            if (f.set(filesystem::to_resource(anim)).has_err())
            {
                log::error("Error writing file to disk! {}", m_filenameBuffer);
            }
        }

        // this checkbox disables the animation & gives the animator control over the cubeEnt
        static bool wasAnimationRunning = false;
        if (base::Checkbox("Pause & Edit", &m_animatorHasControl))
        {
            if (m_animatorHasControl == true)
            {
                wasAnimationRunning = anim.running;
                anim.running = false;
            }
            else
            {
                anim.running = wasAnimationRunning;
            }
        }


        //set the animation for the sequencer
        m_sequencer.SetAnimation(&anim);

        //render the sequencer
        Sequencer(&m_sequencer, nullptr, nullptr, &m_selectedEntry, &m_firstFrame,
            sequencer::SEQUENCER_EDIT_STARTEND |
            sequencer::SEQUENCER_ADD |
            sequencer::SEQUENCER_DEL |
            sequencer::SEQUENCER_COPYPASTE |
            sequencer::SEQUENCER_CHANGE_FRAME);

        //finish the Animator window
        base::End();

        //check if the user selected something in the sequencer window
        if (m_selectedEntry != -1 && m_selectedEntry < m_sequencer.GetItemCount())
        {
            //prepare something things for the gizmos
            ////////////////////////////////////////////////////////////////////////

            ImGuiIO& io = ImGui::GetIO(); // get imguis io

            //calc the aspect ratio of the window
            float aspect = io.DisplaySize.x / io.DisplaySize.y;

            //calc the camera projection
            math::mat4 projection = math::perspective(
                math::deg2rad(cam.fov * aspect),
                aspect, cam.nearz, cam.farz);

            //set the rect that the gizmo is going to work in
            gizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

            ////////////////////////////////////////////////////////////////////////


            //get the selected datapoint
            auto& dp = m_sequencer.GetDataPointAt(m_selectedEntry);

            //create a new window for the value editor
            base::Begin(" - Animator - Edit Values");

            // left.x = right[0] etc...
            #define MAP_XYZ_TO_012(XYZ,_012) \
                    (XYZ).x = (_012)[0];\
                    (XYZ).y = (_012)[1];\
                    (XYZ).z = (_012)[2]

            //check if the datapoint is position
            if (std::holds_alternative<position>(dp))
            {
                position& pos = std::get<position>(dp);

                //modify position with Float3 Input
                float v[3]{ pos.x, pos.y, pos.z };
                base::InputFloat3("Position", v);
                MAP_XYZ_TO_012(pos, v);

                // if the animator assumed control over the entity we can also
                // display gizmos and move the entity around itself
                if (m_animatorHasControl)
                {
                    //calculate model matrix
                    math::mat4 model = math::compose(
                        m_cubeEntity.read_component<scale>(),
                        m_cubeEntity.read_component<rotation>(),
                        pos
                    );

                    //manipulate translation
                    gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection),
                        gizmo::OPERATION::TRANSLATE, gizmo::MODE::LOCAL, value_ptr(model));

                    math::extract_translation(model, pos);

                    m_cubeEntity.get_component_handle<position>().write(pos);
                }
                if(m_applyCopy)
                {
                    //if P was pressed we copy the position of the camera
                    pos = m_copiedPosition;
                    m_applyCopy = false;
                }
            }

            //check if dp is rotation
            if (std::holds_alternative<rotation>(dp))
            {
                rotation& rot = std::get<rotation>(dp);

                //for the orientation we need to convert to euler first, since most people
                //are not native in editing quaternions directly
                auto euler = math::eulerAngles(rot);

                float v[3]{ euler.x, euler.y, euler.z };
                base::InputFloat3("Euler", v);
                MAP_XYZ_TO_012(euler, v);


                if (m_animatorHasControl)
                {
                    math::mat4 model = math::compose(
                        m_cubeEntity.read_component<scale>(),
                        math::quat(euler), // here we need to recalc the quat
                        m_cubeEntity.read_component<position>()
                    );

                    //manipulate orientation
                    gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection),
                        gizmo::OPERATION::ROTATE, gizmo::MODE::LOCAL, value_ptr(model));

                    math::extract_orientation(model, rot);

                    m_cubeEntity.get_component_handle<rotation>().write(rot);
                }
                if(m_applyCopy)
                {
                    //if P was pressed we copy the rotation of the camera
                    rot = m_copiedRotation;
                    m_applyCopy = false;
                }

            }

            //check if dp is scale
            if (std::holds_alternative<scale>(dp))
            {
                scale& s = std::get<scale>(dp);

                float v[3]{ s.x, s.y, s.z };
                base::InputFloat3("Scale", v);
                MAP_XYZ_TO_012(s, v);

                if (m_animatorHasControl)
                {
                    math::mat4 model = math::compose(s, m_cubeEntity.read_component<rotation>(),
                        m_cubeEntity.read_component<position>());

                    //manipulate scale
                    gizmo::Manipulate(value_ptr(cInput.view), value_ptr(projection),
                        gizmo::OPERATION::SCALE, gizmo::MODE::LOCAL, value_ptr(model));

                    math::extract_scale(model, s);

                    m_cubeEntity.get_component_handle<scale>().write(s);
                }
            }
            //check if the data contains an event
            if (std::holds_alternative<detail::AnimationSequencer::event_t>(dp))
            {
                char vbuffer[512]{ 0 };


                //destructure event data
                auto& [event_pointer, id] = std::get<detail::AnimationSequencer::event_t>(dp);
                bool render_base_gui = true;

                base::Separator();
                std::string preview_v = "Nothing";


                auto itr = ext::detail::g_ReverseAnimationEventDatabase.find(id);
                if (itr != detail::g_ReverseAnimationEventDatabase.end())
                {
                    preview_v = std::string(itr->second);
                }
                else
                {
                    //display warning that this is probably not a valid event
                    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollWithMouse;
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                    ImGui::BeginChild("Warning Child", ImVec2(0, 75), true, window_flags);
                    base::TextColored({ 255,180,0,255 }, "Warning!!");
                    base::TextUnformatted("This id does not have an associated name, saving this is going to produce bad results!");
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }

                //display name and changer!
                if (base::BeginCombo("##Type", preview_v.c_str())) {

                    for (auto& [key, value] : detail::g_AnimationEventDatabase)
                    {
                        //we query the database for entries and display them in a combobox, which makes for a pretty useful chooser
                        if (ImGui::Selectable(std::string(key).c_str(), value == id))
                        {
                            id = value;
                        }
                    }
                    base::EndCombo();
                }


                base::Separator();
                //check if a custom render layer is available
                if (m_guiRenderers.contains(id))
                {
                    render_base_gui = m_guiRenderers[id](id, event_pointer.get());
                }
                if (render_base_gui)
                {
                    //if the generic layer is still required:
                    //display id
                    base::InputScalar("ID", ImGuiDataType_U64, &id);

                    //display raw parameters
                    base::PushItemWidth(200);

                    base::TextUnformatted("Event Parameters");
                    base::Separator();
                    for (const auto& [key, value] : event_pointer->enumerate_params())
                    {
                        memcpy(vbuffer, value.data(), value.size() + 1);
                        base::TextUnformatted(key.c_str());
                        base::SameLine();
                        base::InputText(("##" + key).c_str(), vbuffer, 512);
                        event_pointer->receive_param(key, vbuffer);
                    }

                    base::Separator();

                    //display little dialog to add new entries
                    static char newkbuffer[512]{ 0 }, newvbuffer[512]{ 0 };
                    base::TextUnformatted("Key");
                    base::SameLine();
                    base::InputText("##Key", newkbuffer, 512);
                    base::SameLine();
                    base::TextUnformatted("Value");
                    base::SameLine();
                    base::InputText("##Value", newvbuffer, 512);
                    base::PopItemWidth();

                    if (base::Button("Add"))
                    {
                        event_pointer->receive_param(newkbuffer, newvbuffer);
                    }
                }
            }

            //don't need that little helper macro anymore
            #undef  MAP_XYZ_TO_012

            //finish the value editor window here
            base::End();
        }

        //get the animation out of the sequencer and commit to entity
        anim = m_sequencer.GetAnimation();
        m_cubeEntity.get_component_handle<animation>().write(anim);
    }
}
