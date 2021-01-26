#pragma once
#include <application/application.hpp>

namespace legion::rendering
{
    enum struct light_type : uint
    {
        POINT = 0,
        DIRECTIONAL = 1,
        SPOT = 2
    };

    namespace detail
    {
        struct light_data
        {
            light_type type;
            float attenuation;
            float intensity;
            uint index;
            math::vec3 direction;
            float falloff;
            math::vec3 position;
            float angle;
            math::color color;
        };
    }

    struct light
    {
    private:
        static uint m_lastidx;
    public:
        light();

        const detail::light_data& get_light_data(const ecs::component_handle<position>& pos, const ecs::component_handle<rotation>& rot);

        void set_type(light_type type);
        void set_attenuation(float attenuation);
        void set_intensity(float intensity);
        void set_color(math::color color);
        void set_angle(float angle);
        void set_falloff_power(float power);

        L_NODISCARD static light directional(math::color color = math::colors::white, float intensity = 1.0);
        L_NODISCARD static light spot(math::color color = math::colors::white, float angle = 0.785398f, float intensity = 1.0, float attenuation = 10, float falloff = 3.141592f);
        L_NODISCARD static light point(math::color color = math::colors::white, float intensity = 1.0, float attenuation = 10, float falloff = 3.141592f);


        template <class Archive>
        void serialize(Archive& archive)
        {
            using cereal::make_nvp;

            archive(
                make_nvp("LightType",*reinterpret_cast<uint*>(&m_type)),
                make_nvp("Attenuation",m_attenuation),
                make_nvp("Intensity",m_intensity),
                make_nvp("Index",m_index),
                make_nvp("Direction",m_direction),
                make_nvp("Falloff",m_falloff),
                make_nvp("Position",m_position),
                make_nvp("Angle",m_angle),
                make_nvp("Color",*reinterpret_cast<math::vec4*>(&m_color))
            );
        }

    protected:
        union
        {
            detail::light_data m_lightData;

            struct
            {
                light_type m_type;
                float m_attenuation;
                float m_intensity;
                uint m_index;
                math::vec3 m_direction;
                float m_falloff;
                math::vec3 m_position;
                float m_angle;
                math::color m_color;
            };
        };
    };
}
