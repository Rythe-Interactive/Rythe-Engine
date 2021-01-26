#pragma once
#include <core/math/math.hpp>
#include <core/ecs/archetype.hpp>
#include <core/data/mesh.hpp>
#include <core/logging/logging.hpp>

#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <core/ecs/component_handle.hpp>

#include <core/filesystem/assetimporter.hpp>

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

        L_NODISCARD math::vec3 right()
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::right;
        }

        L_NODISCARD math::vec3 up()
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::up;
        }

        L_NODISCARD math::vec3 forward()
        {
            OPTICK_EVENT();
            return math::toMat3(*this) * math::vec3::forward;
        }

        L_NODISCARD math::mat3 matrix()
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

    struct hierarchy
    {
        ecs::entity_handle parent = world_entity_id;
        ecs::entity_set children;
    };

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

        transform() = default;
        transform(const base::handleGroup& handles) : base(handles) {}

        L_NODISCARD std::tuple<position, rotation, scale> get_local_components();

        L_NODISCARD math::mat4 get_world_to_local_matrix()
        {
            OPTICK_EVENT();
            return math::inverse(get_local_to_world_matrix());
        }

        L_NODISCARD math::mat4 get_local_to_world_matrix();

        L_NODISCARD math::mat4 get_local_to_parent_matrix()
        {
            OPTICK_EVENT();
            auto [position, rotation, scale] = get_local_components();
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

    struct mesh_filter : public mesh_handle
    {
        mesh_filter() = default;
        explicit mesh_filter(const mesh_handle& src) : mesh_handle(src) {}

        bool operator==(const mesh_filter& other) const { return id == other.id; }

        template<class Archive>
        void save(Archive& oa)
        {
            bool debug = false;
            if (id != invalid_id)
                oa(id, cereal::make_nvp("Filepath", get().second.filePath));
            else
            {
                log::error("Deserialized Mesh was missing!");
                std::string missing = "engine://resources/invalid/missing_mesh.obj";
                oa(id, cereal::make_nvp("Filepath", missing));
            }
        }

        template<class Archive>
        void load(Archive& oa)
        {
            std::string filepath;
            oa(id,cereal::make_nvp("Filepath", filepath));
            auto copy = default_mesh_settings;
            copy.contextFolder = filesystem::view(filepath).parent();
            id = MeshCache::create_mesh(filepath, filesystem::view(filepath), copy).id;
        }
    };
}

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
