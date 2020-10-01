#pragma once
#include <audio/detail/engine_include.hpp>
#include <audio/systems/audiosystem.hpp>
#include <audio/components/audio_source.hpp>
#include <audio/components/audio_listener.hpp>
#include <audio/data/importers/audio_importers.hpp>

namespace args::audio
{
    class AudioModule : public Module
    {
    public:
        virtual void setup() override
        {
            addProcessChain("Audio");

            fs::AssetImporter::reportConverter<mp3_audio_loader>(".mp3");

            reportComponentType<audio_source>();
            reportComponentType<audio_listener>();
            reportSystem<AudioSystem>();
        }

        virtual priority_type priority() override
        {
            return 50;
        }
    };
}
