#pragma once
#include <core/core.hpp>
#include <audio/data/audio_segment.hpp>
#include <vector>

namespace legion::audio
{
    namespace detail
    {
        struct channel_data
        {
            channel_data(int channels)
                : channels(channels)
            {

            }

            int channels;
            std::vector<byte_vec> dataPerChannel;

            byte* getLeft()
            {
                return getChannelData(0);
            }

            byte* getRight()
            {
                return getChannelData(1);
            }

            byte* getCenter()
            {
                return getChannelData(2);
            }

            byte* getLeftRear()
            {
                return getChannelData(3);
            }

            byte* getRightRear()
            {
                return getChannelData(4);
            }

            byte* getChannelData(size_type index)
            {
                if (channels < index) return nullptr;
                return dataPerChannel[index].data();
            }

            byte* operator [] (size_type index)
            {
                return dataPerChannel[index].data();
            }

            const byte* operator [] (size_type index) const
            {
                return dataPerChannel[index].data();
            }
        };

        /**
        * @brief function to convert audio data to mono audio data
        * @brief inputData is unedited audio data,
        * @brief dataSize is the complete audio data size,
        * @brief monoData is the out mono data, monoData is assumed to be resized with the correct size (dataSize/channelCount)
        * @brief channels is the channelCount or amount of channels
        * @brief bitsPerSample is the audio resolution (16 bit for mp3, and usually 16 bit for wav)
        */
        void convertToMono(const byte* inputData, int dataSize, byte* monoData, int channels, int bitsPerSample);
        byte* convertToMono(const byte* inputData, int dataSize, int& monoDataSize, int& channels, int bitsPerSample);

        channel_data extractChannels(const byte* inputData, int dataSize, int channels, int bitsPerSamples);

        ALenum getAudioFormat(int channels, int bitsPerSample);

        void createAndBufferAudioData(ALuint* bufferId, int channels, int bitsPerSample, byte* data, int dataSize, int sampleRate);
    }

    struct mp3_audio_loader : public fs::resource_converter<audio_segment, audio_import_settings>
    {
        common::result<audio_segment, fs_error> load_default(const filesystem::basic_resource& resource) override
        {
            return load(resource,audio_import_settings(default_audio_import_settings));
        }
        virtual common::result<audio_segment, fs_error> load(const fs::basic_resource& resource, audio_import_settings&& settings) override;
    };

    struct wav_audio_loader : public fs::resource_converter<audio_segment, audio_import_settings>
    {

        common::result<audio_segment, fs_error> load_default(const filesystem::basic_resource& resource) override
        {
            return load(resource,audio_import_settings(default_audio_import_settings));
        }
        virtual common::result<audio_segment, fs_error> load(const fs::basic_resource& resource, audio_import_settings&& settings) override;

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
