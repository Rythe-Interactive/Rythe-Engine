#pragma once
#include <application/application.hpp>
#include <rendering/data/material.hpp>
#include <rendering/data/framebuffer.hpp>

/**
 * @file camera.hpp
 */

namespace legion::rendering
{
    class OldRenderer;

    /**@class camera
     * @brief Camera component
     */
    struct camera
    {
        friend class OldRenderer;
        friend class Renderer;

        ecs::component<app::window> targetWindow = ecs::component<app::window>{ {}, ecs::world };
        framebuffer renderTarget = framebuffer();
        math::color clearColor = math::colors::cornflower;
        struct camera_input
        {
            camera_input(math::mat4 view, math::mat4 proj, math::vec3 pos, math::vec3 vdir, float nearz, float farz, math::ivec2 viewportSize) :
                view(view), proj(proj), pos(pos), nearz(nearz), vdir(vdir), farz(farz), viewportSize(viewportSize)
            {
            }

            void bind(material_handle& materialHandle) const
            {
                if (materialHandle.has_param<math::mat4>(SV_VIEW))
                    materialHandle.set_param(SV_VIEW, view);
                if (materialHandle.has_param<math::mat4>(SV_PROJECT))
                    materialHandle.set_param(SV_PROJECT, proj);
                if (materialHandle.has_param<math::vec4>(SV_CAMPOS))
                    materialHandle.set_param(SV_CAMPOS, posnearz);
                if (materialHandle.has_param<math::vec4>(SV_VIEWDIR))
                    materialHandle.set_param(SV_VIEWDIR, vdirfarz);
                if (materialHandle.has_param<math::ivec2>(SV_VIEWPORT))
                    materialHandle.set_param(SV_VIEWPORT, viewportSize);
            }

            union
            {
                math::vec4 data[10];
                struct
                {
                    math::mat4 view;
                    math::mat4 proj;
                    union
                    {
                        struct
                        {
                            math::vec3 pos;
                            float nearz;
                        };
                        math::vec4 posnearz;
                    };
                    union
                    {
                        struct
                        {
                            math::vec3 vdir;
                            float farz;
                        };
                        math::vec4 vdirfarz;
                    };
                    math::ivec2 viewportSize;
                };
            };
        };

        float fov, nearz, farz;

        /**@brief Set the projection variables of the camera.
         * @param fov Horizontal field of view in degrees.
         * @param nearz Near plane distance from the camera.
         * @param farz Far plane distance from the camera.
         */
        void set_projection(float fov, float nearz, float farz)
        {
            this->fov = fov;
            this->nearz = nearz;
            this->farz = farz;
        }

        /**@brief Get the projection matrix for a certain aspect ratio
         * @param ratio Aspect ratio (width / height)
         */
        math::mat4 get_projection(float ratio)
        {
            return math::perspective(math::deg2rad(fov * ratio), ratio, farz, nearz);
        }

    };
}
