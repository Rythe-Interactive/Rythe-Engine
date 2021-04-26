#pragma once
#include <core/core.hpp>
#include <audio/systems/audiosystem.hpp>
#include <audio/components/audio_source.hpp>
#include <audio/components/audio_listener.hpp>
#include <audio/data/importers/audio_importers.hpp>

namespace legion::audio
{
    class AudioModule : public Module
    {
    public:
        virtual void setup() override
        {
            fs::AssetImporter::reportConverter<mp3_audio_loader>(".mp3");
            fs::AssetImporter::reportConverter<wav_audio_loader>(".wav");

            registerComponentType<audio_source>();
            registerComponentType <audio_listener>();
            reportSystem<AudioSystem>();
        }

        virtual priority_type priority() override
        {
            return 50;
        }
    };
}
