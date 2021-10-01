#pragma once
#include <chrono>
#include <core/time/time_span.hpp>
#include <core/types/primitives.hpp>

namespace legion::core::time
{
    template<typename precision = fast_time, typename chrono_clock = std::chrono::high_resolution_clock>
    struct stopwatch
    {
    public:
        using time_type = precision;
        using span_type = time_span<time_type>;
        using clock_type = chrono_clock;

    private:
        std::chrono::time_point<clock_type> m_start = clock_type::now();
    public:

        void start() noexcept {
            m_start = clock_type::now();
        }

        void fast_forward(span_type time)
        {
            m_start -= std::chrono::duration_cast<typename clock_type::duration>(time.duration);
        }

        void rewind(span_type time)
        {
            m_start += std::chrono::duration_cast<typename clock_type::duration>(time.duration);
        }

        span_type start_point() const noexcept
        {
            return span_type(m_start.time_since_epoch());
        }

        span_type elapsed_time() const noexcept
        {
            return span_type(clock_type::now() - m_start);
        }

        span_type end() noexcept
        {
            return span_type(clock_type::now() - m_start);
        }

        span_type restart() noexcept
        {
            auto startTime = clock_type::now();
            span_type time(startTime - m_start);
            m_start = startTime;
            return time;
        }
    };

    using timer = stopwatch<>;
}
