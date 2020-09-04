#pragma once
#include <core/engine/module.hpp>
#include <core/defaults/defaultcomponents.hpp>

namespace args::core
{
    class CoreModule : public Module
    {
    public:
        virtual void setup() override
        {
            reportComponentType<position>();
            reportComponentType<rotation>();
            reportComponentType<scale>();
        }

        virtual priority_type priority() override
        {
            return PRIORITY_MAX;
        }

    };
}
