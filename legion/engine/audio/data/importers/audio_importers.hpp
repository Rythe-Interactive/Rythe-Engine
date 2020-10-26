#pragma once
#include <core/core.hpp>
#include <audio/data/audio_segment.hpp>

namespace legion::audio
{
    struct mp3_audio_loader : public fs::resource_converter<audio_segment, audio_import_settings>
    {
        virtual common::result_decay_more<audio_segment, fs_error> load(const fs::basic_resource& resource, audio_import_settings&& settings) override;
    };

    struct wav_audio_loader : public fs::resource_converter<audio_segment, audio_import_settings>
    {
        virtual common::result_decay_more<audio_segment, fs_error> load(const fs::basic_resource& resource, audio_import_settings&& settings) override;

        struct RIFF_Header // 36 Bytes of data for WAV header
        {
            uint8 chunckId[4]; // Contains the chars "RIFF"
            int32 chunckSize;
            uint8 format[4]; // Contains the chars "WAVE"
            struct WAVE_Format
            {
                uint8 subChunckId[4]; // Contains the chars "fmt "
                int32 subchunckSize;
                int16 audioFormat;
                int16 channels; // Stereo or mono
                int32 sampleRate; // Sample rate / frequency
                int32 byteRate;
                int16 blockAlign;
                int16 bitsPerSample; // Audio resolution
            } wave_format;
        };

        struct WAVE_Data
        {
            uint8 subChunckId[4]; // Contains the chars "data"
            int32 subChunck2Size; // Sample data
        };
    };
}
