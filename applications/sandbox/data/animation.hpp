#pragma once
#include <vector>
#include <core/core.hpp>
#include <optional>

namespace ext
{
    using namespace legion;

    struct animation_event_base;

    template <class T>
    struct animation_event;


    struct animation_event_base
    {
        void receive_param(const std::string& key, const std::string& value)
        {
            m_params[key] = value;
        }
        std::string get_param(const std::string& key)
        {
            if (auto itr = m_params.find(key); itr != m_params.end())
            {
                return itr->second;
            }
            return "";
        }
        const std::unordered_map<std::string,std::string>& enumerate_params() const
        {
            return m_params;
        }

        events::event_base* surrogate_ctor() const;

    protected:
        friend struct animation;
        std::unordered_map<std::string, std::string> m_params;

    };


    template <class T>
    struct animation_event : public events::event<T>, animation_event_base
    {
    };

    struct void_animation_event : animation_event<void_animation_event>{};

    inline events::event_base* animation_event_base::surrogate_ctor() const
    {
        void_animation_event* t = new void_animation_event();
        t->m_params = this->m_params;
        return t;
    }

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
}
