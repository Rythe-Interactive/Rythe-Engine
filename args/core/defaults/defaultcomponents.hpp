#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>

#include <core/logging/logging.hpp>

namespace args::core
{
    struct position : public math::vec3 {};

    struct rotation : public math::quat
    {
        rotation() = default;

        rotation& operator=(const math::quat& other)
        {
            data = other.data;
            return *this;
        }
    };

    struct scale : public math::vec3
    {
        scale()
        {
            x = 1;
            y = 1;
            z = 1;
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
