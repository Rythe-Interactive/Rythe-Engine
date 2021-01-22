#pragma once
#include <vector>
#include <core/core.hpp>
#include <optional>

#include "animation_events.hpp"

namespace ext
{
    using namespace legion;
    /**
     * @brief Represents the animation data as a component
     * @see animator.hpp for how it is used
     *
     */
    struct animation
    {
        using event_t = std::pair<std::shared_ptr<animation_event_base>, id_type>;

        bool running = false; ///< if this is false the animator will not run this animation
        bool looping = false; ///< if this is true the animation will loop around when finished instead of stopping

        /**
         * @brief These store the animation data (duration + payload)
         * @{
         */
        std::vector<std::pair<float, position>>   position_key_frames;
        std::vector<std::pair<float, rotation>>   rotation_key_frames;
        std::vector<std::pair<float, scale>>      scale_key_frames;
        std::vector<std::pair<float, event_t>>    events;

        /**@} */


        //internal data
        /** @internal
         * @{
         */
        index_type p_index = 0;
        index_type r_index = 0;
        index_type s_index = 0;

        index_type e_index = 0;

        float p_accumulator = 0.0f;
        float r_accumulator = 0.0f;
        float s_accumulator = 0.0f;

        float e_accumulator = 0.0f;
        float padding;
        /**@} */

        //load animation from basic_resource
        static void from_resource(animation* anim, const filesystem::basic_resource& resource);

        //save animation to basic_resource
        static void to_resource(filesystem::basic_resource* resource, const animation& anim);

    };


    namespace detail
    {
        extern std::unordered_map<std::string_view, index_type> g_AnimationEventDatabase;
        extern std::unordered_map<index_type, std::string_view> g_ReverseAnimationEventDatabase;
    }

    template <class T CNDOXY(inherits_from<T, animation_event_base> = 0, inherits_from<T, events::event_base> = 0)>
    inline void registerAnimationEvent(std::string_view prettyName)
    {
        detail::g_AnimationEventDatabase[prettyName] = T::id;
        detail::g_ReverseAnimationEventDatabase[T::id] = prettyName;
    }

    inline common::result<id_type, void> getRegisteredAnimationEventID(std::string_view prettyName)
    {
        using common::Ok, common::Err;

        const auto itr = detail::g_AnimationEventDatabase.find(prettyName);
        if (itr == detail::g_AnimationEventDatabase.end())
        {
            return Err();
        }
        return Ok(itr->second);
    }

}
