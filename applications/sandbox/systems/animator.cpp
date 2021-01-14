#include "animator.hpp"

namespace ext
{
    void Animator::onUpdate(time::span delta)
    {
        //get all entities with animation + transform
        m_query.queryEntities();
        for (ecs::entity_handle ent : m_query)
        {
            //get the animation data from the entity
            animation anim = ent.read_component<animation>();


            //Animation disabled, early out
            if (!anim.running) continue;


            //introduce a little helper to stop repeating the code that is the same for all animation types
            auto advance_accumulator = [&delta, &anim](float& acc, float duration, index_type& index,
                                                       index_type maxIndex, auto onAnimationEnd)
            {
                //increase the accumulator by the passed time / duration that the animation should run for
                acc += delta.seconds() / duration;

                //check if the accumulator is equal or above 1 (aka the animation cycle completed)
                if (acc >= 1.0f)
                {
                    //reset accumulator and advance index
                    acc = 0.0f;
                    index++;
                    if (index >= maxIndex)
                    {
                        if (anim.looping)
                            //start the animation from the beginning if the animation is set to loop
                            index = 0;
                        else
                        {
                            //end the animation when the animation is set to not loop
                            anim.running = false;
                            onAnimationEnd();
                            //signal that the animation has ended
                            return false;
                        }
                    }
                }
                return true;
            };

            //check if we have position key frames
            if (!anim.position_key_frames.empty())
            {
                // get a handle to the position component
                auto position_handle = ent.get_component_handle<position>();


                // if the index is 0 and we do not loop
                // we set the initial position to the first keyframe
                // and skip it
                // this also means that the duration field for the first keyframe is ignored
                if (!anim.looping && anim.p_index == 0)
                {
                    position_handle.write(anim.position_key_frames[0].second);
                    anim.p_index++;
                }

                // we introduce a functor here to set the final position of the animation if it ends,
                // this makes sure that at the end the object is exactly where we need it
                auto write_last_position_key_frame = [&position_handle, &anim]
                {
                    position_handle.write(anim.position_key_frames[anim.position_key_frames.size() - 1].second);
                };

                // the duration of the current animation cycle is stored as the first element of the tuple
                // the position we want to get to is stored as the second element
                const auto& [duration, position] = anim.position_key_frames[anim.p_index];

                // here we invoke the helper defined above
                // it handles all the accumulator logic
                if (!advance_accumulator(anim.p_accumulator, duration, anim.p_index, anim.position_key_frames.size(),
                                         write_last_position_key_frame))
                {
                    // if the helper returns false, it means that the animation has ended
                    // we should therefore not continue any calculations
                    continue;
                }


                // calculate the index of the previous frame (if the current index is 0 ,it will be the last one)
                index_type prev_index = anim.p_index == 0 ? anim.position_key_frames.size() - 1 : anim.p_index - 1;

                // lerp between the current and the previous position using the accumulator
                auto newPos = math::lerp(anim.position_key_frames[prev_index].second, position, anim.p_accumulator);

                // write that position to the entity
                position_handle.write(newPos);
            }

            //check if we have rotation keyframes
            if (!anim.rotation_key_frames.empty())
            {
                // get a handle to the rotation component
                auto rotation_handle = ent.get_component_handle<rotation>();

                // skip first keyframe if not looping and index 0
                if (!anim.looping && anim.r_index == 0)
                {
                    rotation_handle.write(anim.rotation_key_frames[0].second);
                    anim.r_index++;
                }

                // helper to write last rotation keyframe on animation end
                auto write_last_rotation_keyframe = [&rotation_handle, &anim]
                {
                    rotation_handle.write(anim.rotation_key_frames[anim.rotation_key_frames.size() - 1].second);
                };

                const auto& [duration, rotation] = anim.rotation_key_frames[anim.r_index];

                // invoke helper just like for position (see line 95-102)
                if (!advance_accumulator(anim.r_accumulator, duration, anim.r_index, anim.rotation_key_frames.size(),
                                         write_last_rotation_keyframe))
                {
                    continue;
                }

                //calculate new rotation & commit to entity
                index_type prev_index = anim.r_index == 0 ? anim.rotation_key_frames.size() - 1 : anim.r_index - 1;
                auto newRot = math::slerp(anim.rotation_key_frames[prev_index].second, rotation, anim.r_accumulator);
                //                  ^^~~~~~~~~~~|------------------------|
                //NOTE(algo-ryth-mix): usage of slerp instead of lerp here, otherwise the results for quaternions are weird
                rotation_handle.write(newRot);
            }

            if (!anim.scale_key_frames.empty())
            {
                // get handle for reading and writing to scale
                auto scale_handle = ent.get_component_handle<scale>();

                //prepare non-looping initial case
                if (!anim.looping && anim.s_index == 0)
                {
                    scale_handle.write(anim.scale_key_frames[0].second);
                    anim.s_index++;
                }

                //prepare helper for writing last scale
                auto write_last_scale_key_frame = [&scale_handle,&anim]
                {
                    scale_handle.write(anim.scale_key_frames[anim.scale_key_frames.size() - 1].second);
                };

                const auto& [duration,scale] = anim.scale_key_frames[anim.s_index];

                //do accumulator calculation
                if (!advance_accumulator(anim.s_accumulator, duration, anim.s_index, anim.scale_key_frames.size(),
                                         write_last_scale_key_frame))
                {
                    continue;
                }

                //calc new scale and commit to entity
                index_type prev_index = anim.s_index == 0 ? anim.scale_key_frames.size() - 1 : anim.s_index - 1;
                auto newSc = math::lerp(anim.scale_key_frames[prev_index].second, scale, anim.s_accumulator);
                scale_handle.write(newSc);
            }
            if(!anim.events.empty())
            {

                //advance event accumulator (since events only trigger, they do not need a duration)
                anim.e_accumulator = anim.e_accumulator + delta.seconds();

                //check if all events triggered and reset if requested
                if(anim.e_index >= anim.events.size())
                {
                    if(anim.looping)
                    {
                        anim.e_accumulator = 0.0f;
                        anim.e_index = 0;
                    }
                }
                else
                {
                    //get current event and trigger if the acc is above the threshold
                    auto& [trigger_time, event_info] = anim.events[anim.e_index];
                    if(anim.e_accumulator > trigger_time)
                    {
                        auto &[ev,event_id] = event_info;
                        anim.e_index++;
                        m_eventBus->raiseEventUnsafe(std::unique_ptr<events::event_base>(ev->surrogate_ctor()),event_id);
                    }
                }
            }

            //commit animation component to entity
            ent.get_component_handle<animation>().write(anim);
        }
    }
}
