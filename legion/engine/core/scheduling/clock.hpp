#pragma once
#include <core/time/time.hpp>

namespace legion::core::scheduling
{
    enum struct advancement_protocol
    {
        Free, Interval, Manual
    };

    class Clock
    {
    public:
        void setAdvancementProtocol(advancement_protocol protocol);

        void setTickSpeed(time::span interval);

        void run();

        void pause(time::span duration);

        void stop();

        void tick();
    };
}
