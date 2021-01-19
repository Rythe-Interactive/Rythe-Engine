#pragma once
namespace legion::core::async
{
    enum struct wait_priority : int { normal, sleep, real_time };

    inline constexpr wait_priority wait_priority_normal = wait_priority::normal;
    inline constexpr wait_priority wait_priority_sleep = wait_priority::sleep;
    inline constexpr wait_priority wait_priority_real_time = wait_priority::real_time;
}
