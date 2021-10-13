#pragma once
#include <core/math/math.hpp>
#include <core/ecs/ecs.hpp>
#include <core/data/mesh.hpp>
#include <core/logging/logging.hpp>


namespace legion::core
{
    struct position : public math::vec3
    {
        position() : math::vec3(0, 0, 0) {}
        position(const position&) = default;
        position(position&&) = default;
        position(const math::vec3& src) : math::vec3(src) {}
        position(float x, float y, float z) : math::vec3(x, y, z) {}
        position(float v) : math::vec3(v) {}
        position& operator=(const position&) = default;
        position& operator=(position&&) = default;
        position& operator=(const math::vec3& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }
        position& operator=(math::vec3&& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }
    };

    struct rotation : public math::quat
    {
        rotation() : math::quat(1, 0, 0, 0) {}
        rotation(float w, float x, float y, float z) : math::quat(w, x, y, z) {}
        rotation(const rotation&) = default;
        rotation(rotation&&) = default;
        rotation(const math::quat& src) : math::quat(src) {}
        rotation& operator=(const rotation&) = default;
        rotation& operator=(rotation&&) = default;
        rotation& operator=(const math::quat& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            w = src.w;
            return *this;
        }
        rotation& operator=(math::quat&& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            w = src.w;
            return *this;
        }

        L_NODISCARD math::vec3 right() const
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::right;
        }

        L_NODISCARD math::vec3 up() const
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::up;
        }

        L_NODISCARD math::vec3 forward() const
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::forward;
        }

        L_NODISCARD math::mat3 matrix() const
        {
            OPTICK_EVENT();
            return math::toMat3(*this);
        }

        L_NODISCARD static rotation lookat(math::vec3 position, math::vec3 center, math::vec3 up = math::vec3::up);
    };

    L_NODISCARD inline rotation rotation::lookat(math::vec3 position, math::vec3 center, math::vec3 up)
    {
        OPTICK_EVENT();
        return math::conjugate(math::normalize(math::toQuat(math::lookAt(position, center, up))));
    }

    struct scale : public math::vec3
    {
        scale() : math::vec3(1, 1, 1) {}
        scale(float x, float y, float z) : math::vec3(x, y, z) {}
        scale(float v) : math::vec3(v) {}
        scale(const scale&) = default;
        scale(scale&&) = default;
        scale(const math::vec3& src) : math::vec3(src) {}
        scale& operator=(const scale&) = default;
        scale& operator=(scale&&) = default;
        scale& operator=(const math::vec3& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }
        scale& operator=(math::vec3&& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }

    };

    struct transform : public ecs::archetype<position, rotation, scale>
    {
        using base = ecs::archetype<position, rotation, scale>;
        using base::archetype;

        L_NODISCARD math::mat4 from_world_matrix()
        {
            return math::inverse(to_world_matrix());
        }

        L_NODISCARD math::mat4 to_world_matrix()
        {
            OPTICK_EVENT();
            if (owner->parent)
            {
                transform parentTrans = owner->parent.get_component<transform>();
                if (parentTrans)
                    return parentTrans.to_world_matrix() * to_parent_matrix();
            }
            return to_parent_matrix();
        }

        L_NODISCARD math::mat4 from_parent_matrix()
        {
            return math::inverse(to_parent_matrix());
        }

        L_NODISCARD math::mat4 to_parent_matrix()
        {
            OPTICK_EVENT();
            auto [position, rotation, scale] = values();
            return math::compose(scale, rotation, position);
        }

    };

    struct velocity : public math::vec3
    {
        velocity() : math::vec3(0, 0, 0) {}
        velocity(const velocity&) = default;
        velocity(velocity&&) = default;
        velocity(const math::vec3& src) : math::vec3(src) {}
        velocity(float x, float y, float z) : math::vec3(x, y, z) {}
        velocity(float v) : math::vec3(v) {}
        velocity& operator=(const velocity&) = default;
        velocity& operator=(velocity&&) = default;
        velocity& operator=(const math::vec3& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }
        velocity& operator=(math::vec3&& src)
        {
            x = src.x;
            y = src.y;
            z = src.z;
            return *this;
        }
    };

    struct mesh_filter
    {
        assets::asset<mesh> shared_mesh;

        explicit mesh_filter(const assets::asset<mesh>& src) : shared_mesh(src) {}

        RULE_OF_5_NOEXCEPT(mesh_filter);

        bool operator==(const mesh_filter& other) const { return shared_mesh == other.shared_mesh; }
    };
}

ManualReflector(legion::core::position, x, y, z);
ManualReflector(legion::core::rotation, x, y, z, w);
ManualReflector(legion::core::scale, x, y, z);
ManualReflector(legion::core::velocity, x, y, z);
ManualReflector(legion::core::mesh_filter, shared_mesh);

#if !defined(DOXY_EXCLUDE)
namespace std // NOLINT(cert-dcl58-cpp)
{
    template <::std::size_t I>
    struct tuple_element<I, legion::core::transform>
    {
        using type = typename legion::core::element_at_t<I, legion::core::position, legion::core::rotation, legion::core::scale>;
    };

    template<>
    struct tuple_size<legion::core::transform>
        : public std::integral_constant<std::size_t, 3>
    {
    };

    template<>
    struct tuple_size<const legion::core::transform>
        : public std::integral_constant<std::size_t, 3>
    {
    };

    template<>
    struct tuple_size<volatile legion::core::transform>
        : public std::integral_constant<std::size_t, 3>
    {
    };

    template<>
    struct tuple_size<const volatile legion::core::transform>
        : public std::integral_constant<std::size_t, 3>
    {
    };

}
#endif

#if !defined(DOXY_EXCLUDE)
namespace fmt
{
    using namespace legion::core;

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
    struct fmt::formatter<velocity> {
        char presentation = 'f';

        constexpr auto parse(format_parse_context& ctx) {

            auto it = ctx.begin(), end = ctx.end();
            if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

            if (it != end && *it != '}')
                throw format_error("invalid format");

            return it;
        }

        template <typename FormatContext>
        auto format(const velocity& p, FormatContext& ctx) {
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
#endif
