#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>

#include <core/logging/logging.hpp>

namespace args::core
{
    struct position : public math::vec3
    {
        position() = default;
        position(const position&) = default;
        position(position&&) = default;
        position(const math::vec3& src) : math::vec3(src) {}
        position(float x, float y, float z) : math::vec3(x, y, z) {}
        position& operator=(const position&) = default;
        position& operator=(position&&) = default;
        position& operator=(const math::vec3& src)
        {
            data = src.data;
            return *this;
        }

        static void init(position& pos)
        {
            pos.data = { 0.f, 0.f, 0.f };
            log::debug("initializing position");
        }

        static void destroy(position&)
        {
            log::debug("destroying position");
        }
    };

    struct rotation : public math::quat
    {
        rotation() : math::quat(1,0,0,0) {}
        rotation(const rotation&) = default;
        rotation(rotation&&) = default;
        rotation(const math::quat& src) : math::quat(src) {}
        rotation& operator=(const rotation&) = default;
        rotation& operator=(rotation&&) = default;
        rotation& operator=(const math::quat& src)
        {
            data = src.data;
            return *this;
        }
    };

    struct scale : public math::vec3
    {
        scale() : math::vec3(1, 1, 1) {}
        scale(const scale&) = default;
        scale(scale&&) = default;
        scale(const math::vec3& src) : math::vec3(src) {}
        scale& operator=(const scale&) = default;
        scale& operator=(scale&&) = default;
        scale& operator=(const math::vec3& src)
        {
            data = src.data;
            return *this;
        }
    };

    struct transform : public ecs::archetype<position, rotation, scale> {};
}

namespace fmt
{
    using namespace args::core;

    template <>
    struct fmt::formatter<position> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {

            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const position& p, FormatContext& ctx) {
            return format_to(
                ctx.out(),

                presentation == 'f' ? "{:f}" : "{:e}",
                static_cast<math::vec3>(p));
        }
    };
    template <>
    struct fmt::formatter<rotation> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {

            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const rotation& r, FormatContext& ctx) {
            return format_to(
                ctx.out(),

                presentation == 'f' ? "{:f}" : "{:e}",
                static_cast<math::quat>(r));
        }
    };
    template <>
    struct fmt::formatter<scale> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {

            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const scale& s, FormatContext& ctx) {
            return format_to(
                ctx.out(),

                presentation == 'f' ? "{:f}" : "{:e}",
                static_cast<math::vec3>(s));
        }
    };
}
