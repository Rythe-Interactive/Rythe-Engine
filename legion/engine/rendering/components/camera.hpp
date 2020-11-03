#pragma once
#include <application/application.hpp>
#include <rendering/data/material.hpp>
namespace legion::rendering
{
    struct camera
    {
        struct camera_input
        {
            camera_input(math::mat4 view, math::mat4 proj, math::vec3 pos, uint idx, math::vec3 vdir) :
                view(view), proj(proj), pos(pos), meta1(0), vdir(vdir), meta2(0), idx(idx)
            {
            }

            void bind(material_handle& materialHandle)
            {
                if (materialHandle.has_param<math::mat4>(SV_VIEW))
                    materialHandle.set_param(SV_VIEW, view);
                if (materialHandle.has_param<math::mat4>(SV_PROJECT))
                    materialHandle.set_param(SV_PROJECT, proj);
                if (materialHandle.has_param<math::vec4>(SV_CAMPOS))
                    materialHandle.set_param(SV_CAMPOS, posmeta);
                if (materialHandle.has_param<math::vec4>(SV_VIEWDIR))
                    materialHandle.set_param(SV_VIEWDIR, vdirmeta);
                if (materialHandle.has_param<uint>(SV_CAMIDX))
                    materialHandle.set_param(SV_CAMIDX, idx);
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
                            float meta1;
                        };
                        math::vec4 posmeta;
                    };
                    union
                    {
                        struct
                        {
                            math::vec3 vdir;
                            float meta2;
                        };
                        math::vec4 vdirmeta;
                    };
                    uint idx;
                };
            };
        };

        float fov, nearz, farz;

        void set_projection(float fov, float nearz, float farz)
        {
            this->fov = fov;
            this->nearz = nearz;
            this->farz = farz;
        }

        math::mat4 get_projection(float ratio)
        {
            return math::perspective(math::deg2rad(fov * ratio), ratio, farz, nearz);
        }
    };
}
