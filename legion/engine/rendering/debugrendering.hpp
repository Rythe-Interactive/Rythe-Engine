#pragma once

#include <core/core.hpp>

namespace legion::debug
{
#if !defined drawLine

#define debug_line_event CONCAT(PROJECT_NAME, debug_line)

    struct debug_line_event final : public events::event_base
    {
        math::float3 start;
        math::float3 end;
        math::color color = math::colors::white;
        float width = 1.f;
        float time = 0;
        mutable float timeBuffer = 0;
        bool ignoreDepth = false;

        debug_line_event(math::float3 start, math::float3 end, math::color color = math::colors::white, float width = 1.f, float time = 0, bool ignoreDepth = false) : start(start), end(end), color(color), width(width), time(time), ignoreDepth(ignoreDepth) {}
        debug_line_event() = default;
        debug_line_event(const debug_line_event&) = default;
        debug_line_event(debug_line_event&&) = default;

        debug_line_event& operator=(const debug_line_event&) = default;
        debug_line_event& operator=(debug_line_event&&) = default;

        bool operator==(const debug_line_event& other) const
        {
            return start == other.start && end == other.end && color == other.color && width == other.width && ignoreDepth == other.ignoreDepth;
        }

        virtual id_type get_id()
        {
            static id_type id = nameHash("debug_line");
            return id;
        }
    };

#define drawLine CONCAT(PROJECT_NAME, DrawLine)

    inline void drawLine(math::float3 start, math::float3 end, math::color color = math::colors::white, float width = 1.f, float time = 0, bool ignoreDepth = false)
    {
        auto val = debug_line_event(start, end, color, width, time, ignoreDepth);
        events::EventBus::raiseEvent(val);
    }

#define drawCube CONCAT(PROJECT_NAME, DrawCube)

    inline void drawCube(math::float3 min, math::float3 max, math::color color = math::colors::white, float width = 1.f, float time = 0, bool ignoreDepth = false)
    {
        //draws all 12 cube edges 
        drawLine(min, math::float3(max.x, min.y, min.z), color, width, time, ignoreDepth);
        drawLine(min, math::float3(min.x, max.y, min.z), color, width, time, ignoreDepth);
        drawLine(min, math::float3(min.x, min.y, max.z), color, width, time, ignoreDepth);
        drawLine(math::float3(min.x, max.y, max.z), max, color, width, time, ignoreDepth);
        drawLine(math::float3(max.x, max.y, min.z), max, color, width, time, ignoreDepth);
        drawLine(math::float3(max.x, min.y, max.z), max, color, width, time, ignoreDepth);
        drawLine(math::float3(max.x, min.y, min.z), math::float3(max.x, max.y, min.z), color, width, time, ignoreDepth);
        drawLine(math::float3(max.x, min.y, min.z), math::float3(max.x, min.y, max.z), color, width, time, ignoreDepth);
        drawLine(math::float3(min.x, max.y, min.z), math::float3(max.x, max.y, min.z), color, width, time, ignoreDepth);
        drawLine(math::float3(min.x, max.y, min.z), math::float3(min.x, max.y, max.z), color, width, time, ignoreDepth);
        drawLine(math::float3(min.x, min.y, max.z), math::float3(max.x, min.y, max.z), color, width, time, ignoreDepth);
        drawLine(math::float3(min.x, min.y, max.z), math::float3(min.x, max.y, max.z), color, width, time, ignoreDepth);
    }

#endif


}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::debug::debug_line_event>
    {
        std::size_t operator()(legion::debug::debug_line_event const& line) const noexcept
        {
            std::hash<legion::core::math::float3> vecHasher;
            std::hash<legion::core::math::color> colHasher;
            std::hash<float> fltHasher;
            std::hash<bool> boolHasher;

            size_t seed = 0;
            legion::core::math::detail::hash_combine(seed, vecHasher(line.start));
            legion::core::math::detail::hash_combine(seed, vecHasher(line.end));
            legion::core::math::detail::hash_combine(seed, colHasher(line.color));
            legion::core::math::detail::hash_combine(seed, fltHasher(line.width));
            legion::core::math::detail::hash_combine(seed, boolHasher(line.ignoreDepth));
            return seed;
        }
    };
}
#endif
