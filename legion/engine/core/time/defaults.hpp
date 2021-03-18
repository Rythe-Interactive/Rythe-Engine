#pragma once
#include <core/time/time_span.hpp>
#include <core/time/stopwatch.hpp>

namespace legion::core::time
{
    extern const stopwatch<main_clock_time> mainClock;
}
