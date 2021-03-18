#pragma once
#include <chrono>
#include <core/time/time_span.hpp>
#include <core/types/primitives.hpp>

namespace legion::core::time
{
    template<typename time_type = time64>
    struct stopwatch
    {
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start = std::chrono::high_resolution_clock::now();
    public:

        void start() noexcept {
            m_start = std::chrono::high_resolution_clock::now();
        }

        time_span<time_type> startPoint() const noexcept
        {
            return m_start.time_since_epoch();
        }

        time_span<time_type> elapsedTime() const noexcept
        {
            return time_span<time_type>(std::chrono::high_resolution_clock::now() - m_start);
        }

        time_span<time_type> end() noexcept
        {
            return time_span<time_type>(std::chrono::high_resolution_clock::now() - m_start);
        }

        time_span<time_type> restart() noexcept
        {
            auto startTime = std::chrono::high_resolution_clock::now();
            time_span<time_type> time(startTime - m_start);
            m_start = startTime;
            return time;
        }
    };

    using timer = stopwatch<>;
}
