#pragma once
#include <audio/detail/engine_include.hpp>
#include <audio/systems/audiosystem.hpp>

namespace args::audio
{
    class AudioModule : public Module
    {
    public:
        virtual void setup() override
        {
            addProcessChain("Audio");

            //reportComponentType<sah>();
            reportSystem<AudioSystem>();
        }

        virtual priority_type priority() override
        {
            return default_priority;
        }
    };
}
