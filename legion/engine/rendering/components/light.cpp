#include <rendering/components/light.hpp>

namespace legion::rendering
{
    uint light::m_lastidx;

    light::light() : m_type(light_type::POINT), m_attenuation(10), m_intensity(1), m_index(m_lastidx++), m_direction(0, 0, 1), m_falloff(math::pi<float>()), m_position(0,0,0), m_angle(math::radians(45.f)), m_color(1, 1, 1) { }

    const detail::light_data& light::get_light_data(const ecs::component<position>& pos, const ecs::component<rotation>& rot)
    {
        if (m_type != light_type::DIRECTIONAL)
            m_position = pos;
        else
            m_attenuation = FLT_MAX;

        if (m_type != light_type::POINT)
            m_direction = -rot->forward();

        return m_lightData;
    }

    void light::set_type(light_type type)
    {
        m_type = type;
    }

    void light::set_attenuation(float attenuation)
    {
        m_attenuation = attenuation;
    }

    void light::set_intensity(float intensity)
    {
        m_intensity = intensity;
    }

    void light::set_color(math::color color)
    {
        m_color = color;
    }

    void light::set_angle(float angle)
    {
        m_angle = angle;
    }

    void light::set_falloff_power(float power)
    {
        m_falloff = power;
    }

    L_NODISCARD light light::directional(math::color color, float intensity)
    {
        light ret;
        ret.set_type(light_type::DIRECTIONAL);
        ret.set_color(color);
        ret.set_intensity(intensity);
        return ret;
    }

    L_NODISCARD light light::spot(math::color color, float angle, float intensity, float attenuation, float falloff)
    {
        light ret;
        ret.set_type(light_type::SPOT);
        ret.set_angle(angle);
        ret.set_color(color);
        ret.set_intensity(intensity);
        ret.set_attenuation(attenuation);
        ret.set_falloff_power(falloff);
        return ret;
    }

    L_NODISCARD light light::point(math::color color, float intensity, float attenuation, float falloff)
    {
        light ret;
        ret.set_type(light_type::POINT);
        ret.set_color(color);
        ret.set_intensity(intensity);
        ret.set_attenuation(attenuation);
        ret.set_falloff_power(falloff);
        return ret;
    }

}
