#pragma once
#include <vector>
#include <core/core.hpp>
#include <optional>

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
        bool running = false; ///< if this is false the animator will not run this animation
        bool looping = false; ///< if this is true the animation will loop around when finished instead of stopping

        /**
         * @brief These store the animation data (duration + payload)
         * @{
         */
        std::vector<std::pair<float, position>>   position_key_frames;
        std::vector<std::pair<float, rotation>>   rotation_key_frames;
        std::vector<std::pair<float, scale>>      scale_key_frames;
        /**@} */


        //internal data
        /** @internal
         * @{
         */
        index_type p_index = 0;
        index_type r_index = 0;
        index_type s_index = 0;

        float p_accumulator = 0.0f;
        float r_accumulator = 0.0f;
        float s_accumulator = 0.0f;
        /**@} */

        //load animation from basic_resource
        static void from_resource(animation* anim, const filesystem::basic_resource& resource);

        //save animation to basic_resource
        static void to_resource(filesystem::basic_resource* resource, const animation& value);

    };
}
