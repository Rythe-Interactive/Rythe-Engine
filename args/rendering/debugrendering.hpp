#pragma once

#if !defined(ARGS_API)
    #if !defined(ARGS_IMPORT)
        #define ARGS_IMPORT
        #include <core/core.hpp>
        #include <core/platform/args_library.hpp>
    #else
        #include <core/core.hpp>
    #endif
#endif

namespace args::debug
{
    namespace detail
    {
        events::EventBus* eventBus = nullptr;
    }

    struct debug_line final : public events::event<debug_line>
    {
        math::vec3 start;
        math::vec3 end;
        math::color color = math::colors::white;
        float width = 1.f;
        bool ignoreDepth = false;

        debug_line(math::vec3 start, math::vec3 end, math::color color = math::colors::white, float width = 1.f, bool ignoreDepth = false) : start(start), end(end), color(color), width(width), ignoreDepth(ignoreDepth) {}
        debug_line() = default;
        debug_line(const debug_line&) = default;
        debug_line(debug_line&&) = default;

        debug_line& operator=(const debug_line&) = default;
        debug_line& operator=(debug_line&&) = default;

        bool operator==(const debug_line& other) const
        {
            return start == other.start && end == other.end && color == other.color && width == other.width && ignoreDepth == other.ignoreDepth;
        }

        virtual bool unique() override { return false; }
    };

    void drawLine(math::vec3 start, math::vec3 end, math::color color = math::colors::white, float width = 1.f, bool ignoreDepth = false)
    {
        if (!detail::eventBus)
            return;

        detail::eventBus->raiseEvent<debug_line>(start, end, color, width, ignoreDepth);
    }
}

namespace std
{
    template<>
    struct hash<args::debug::debug_line>
    {
        std::size_t operator()(args::debug::debug_line const& line) const noexcept
        {
            std::hash<args::core::math::vec3> vecHasher;
            std::hash<args::core::math::color> colHasher;
            std::hash<float> fltHasher;
            std::hash<bool> boolHasher;

            size_t seed = 0;
            args::core::math::detail::hash_combine(seed, vecHasher(line.start));
            args::core::math::detail::hash_combine(seed, vecHasher(line.end));
            args::core::math::detail::hash_combine(seed, colHasher(line.color));
            args::core::math::detail::hash_combine(seed, fltHasher(line.width));
            args::core::math::detail::hash_combine(seed, boolHasher(line.ignoreDepth));
            return seed;
        }
    };
}
