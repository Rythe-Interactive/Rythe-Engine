#pragma once
#include <core/events/event.hpp>

namespace args::core::events
{
    struct exit final : public event<exit>
    {
        const int exitcode;

        exit(int exitcode = 0) : exitcode(exitcode) {}

        virtual bool persistent() { return true; }

    };
}
