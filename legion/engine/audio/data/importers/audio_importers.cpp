#include <audio/data/importers/audio_importers.hpp>
#if !defined(DOXY_EXCLUDE)
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>
#endif
#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    common::result<audio_segment, fs_error> mp3_audio_loader::load(const fs::basic_resource& resource, audio_import_settings&& settings)
    {

        mp3dec_map_info_t map_info;
        map_info.buffer = resource.data();
        map_info.size = resource.size();

        mp3dec_t mp3dec;
        mp3dec_file_info_t fileInfo;

        if (mp3dec_load_mapinfo(&mp3dec, &map_info, &fileInfo, NULL, NULL))
        {
            return legion_fs_error("Failed to load audio file");
        }

        byte* audioData;

        // bitsPerSample is always 16 for mp3
        int dataSize = fileInfo.samples * sizeof(int16);
        int channels = fileInfo.channels;
        int samples = fileInfo.samples;

        if (settings.channel_processing == audio_import_settings::channel_processing_setting::force_mono)
        {
            audioData = detail::convertToMono(reinterpret_cast<byte*>(fileInfo.buffer), dataSize, dataSize, channels, 16);
            samples /= channels;
        }
        else
        {
            audioData = new byte[dataSize];
            memmove(audioData, fileInfo.buffer, dataSize);
        }
        free(fileInfo.buffer);

        audio_segment as(
            audioData, // fileInfo.samples is int16, therefore byte requires twice as much
            0,
            samples,
            channels,
            fileInfo.hz,
            fileInfo.layer,
            fileInfo.avg_bitrate_kbps
        );

        std::lock_guard guard(AudioSystem::contextLock);
        alcMakeContextCurrent(AudioSystem::alcContext);
        //Generate openal buffer
        alGenBuffers((ALuint)1, &as.audioBufferId);

        ALenum format = AL_FORMAT_MONO16;
        if (as.channels == 2) format = AL_FORMAT_STEREO16;

        alBufferData(as.audioBufferId, format, as.getData(), dataSize, as.sampleRate);

        alcMakeContextCurrent(nullptr);

        return as;
    }

    common::result<audio_segment, fs_error> wav_audio_loader::load(const fs::basic_resource& resource, audio_import_settings&& settings)
    {
        RIFF_Header header;
        WAVE_Data waveData;

        memcpy(&header, resource.data(), sizeof(header)); // Copy header data into the header struct

        // Check if the loaded file has the correct header

        if (header.chunckId[0] != 'R' ||
            header.chunckId[1] != 'I' ||
            header.chunckId[2] != 'F' ||
            header.chunckId[3] != 'F')
        {
            log::error("Found WAV header: '{}', exptected: 'RIFF'", (char)header.chunckId[0], (char)header.chunckId[1], (char)header.chunckId[2], (char)header.chunckId[3]);
            return legion_fs_error("WAV File invalid header, exptected RIFF");
        }

        if (header.format[0] != 'W' ||
            header.format[1] != 'A' ||
            header.format[2] != 'V' ||
            header.format[3] != 'E')
        {
            log::error("Found WAV format: '{}{}{}{}', exptected: 'WAVE'", (char)header.format[0], (char)header.format[1], (char)header.format[2], (char)header.format[3]);
            return legion_fs_error("Loaded File is not of type WAV");
        }

        if (header.wave_format.subChunckId[0] != 'f' ||
            header.wave_format.subChunckId[1] != 'm' ||
            header.wave_format.subChunckId[2] != 't' ||
            header.wave_format.subChunckId[3] != ' ')
        {
            log::error("Found WAV format sub chunck ID: '{}{}{}{}', exptected: 'fmt '", (char)header.wave_format.subChunckId[0], (char)header.wave_format.subChunckId[1], (char)header.wave_format.subChunckId[2], (char)header.wave_format.subChunckId[3]);
            return legion_fs_error("WAV File sub chunck id was not (fmt )");
        }

        memcpy(&waveData, resource.data() + sizeof(header), sizeof(waveData));
        if (waveData.subChunckId[0] != 'd' ||
            waveData.subChunckId[1] != 'a' ||
            waveData.subChunckId[2] != 't' ||
            waveData.subChunckId[3] != 'a')
        {
            log::error("Found WAV data sub chunck ID: '{}{}{}{}', exptected: 'data'", (char)waveData.subChunckId[0], (char)waveData.subChunckId[1], (char)waveData.subChunckId[2], (char)waveData.subChunckId[3]);
            return legion_fs_error("WAV File sample data does not start with word (data)");
        }

        assert_msg("Audio file channels were 0", header.wave_format.channels != 0);

        uint metaSize = sizeof(header) + sizeof(waveData);

        int sampleDataSize = resource.size() - metaSize;
        byte* audioData;

        int channels = header.wave_format.channels;

        audio_segment as;

        if (settings.channel_processing == audio_import_settings::channel_processing_setting::split_channels)
        {
            detail::channel_data channelData = detail::extractChannels(resource.data() + metaSize, sampleDataSize, channels, header.wave_format.bitsPerSample);

            sampleDataSize /= channels;

            audioData = new byte[sampleDataSize];
            memmove(audioData, channelData.dataPerChannel[0].data(), sampleDataSize);
            //audioData = channelData.dataPerChannel[0].data();

            int samplesPerChannel = sampleDataSize / (header.wave_format.bitsPerSample / 8);

            as = audio_segment(
                audioData,
                0,
                samplesPerChannel, // Sample count, unknown for wav
                1,
                (int)header.wave_format.sampleRate,
                -1, // Layer, does not exist in wav
                -1 // avg_biterate_kbps, unknown for wav
            );

            audio_segment* previous = &as;

            if (channels > 1)
            {
                for (int i = 1; i < channels; ++i)
                {
                    byte* data = new byte[sampleDataSize];
                    memmove(data, channelData.dataPerChannel[i].data(), sampleDataSize);

                    audio_segment* channel_segment = new audio_segment(
                        data,
                        0,
                        samplesPerChannel,
                        1,
                        (int)header.wave_format.sampleRate,
                        -1,
                        -1
                    );

                    detail::createAndBufferAudioData(&(channel_segment->audioBufferId), channel_segment->channels, header.wave_format.bitsPerSample, channel_segment->getData(), sampleDataSize, channel_segment->sampleRate);
                    log::debug("Setting next audio segment");
                    previous->setNextAudioSegment(*channel_segment);
                    previous = channel_segment;
                }
            }
        }
        else if (settings.channel_processing == audio_import_settings::channel_processing_setting::force_mono)
        {
            audioData = detail::convertToMono(resource.data() + metaSize, sampleDataSize, sampleDataSize, channels, header.wave_format.bitsPerSample);

            as = audio_segment(
                audioData,
                0,
                sampleDataSize / (header.wave_format.bitsPerSample / 8), // Sample count, unknown for wav
                channels,
                (int)header.wave_format.sampleRate,
                -1, // Layer, does not exist in wav
                -1 // avg_biterate_kbps, unknown for wav
            );
        }
        else
        {
            audioData = new byte[sampleDataSize];
            memcpy(audioData, resource.data() + metaSize, sampleDataSize);

            as = audio_segment(
                audioData,
                0,
                sampleDataSize / (header.wave_format.bitsPerSample / 8), // Sample count, unknown for wav
                channels,
                (int)header.wave_format.sampleRate,
                -1, // Layer, does not exist in wav
                -1 // avg_biterate_kbps, unknown for wav
            );
        }

        detail::createAndBufferAudioData(&as.audioBufferId, as.channels, header.wave_format.bitsPerSample, as.getData(), sampleDataSize, as.sampleRate);

        return as;
    }

    namespace detail
    {
        void convertToMono(const byte* inputData, int dataSize, byte* monoData, int channels, int bitsPerSample)
        {
            assert_msg("0 was passed for channels", channels != 0);
            if (channels == 1)
            {
                memcpy(monoData, inputData, dataSize);
                return;
            }
            uint bytesPerSample = bitsPerSample / 8;

            uint j = 0;
            uint channelSize = channels * bytesPerSample;
            for (uint i = 0; i < dataSize; i += channelSize)
            {
                switch (bytesPerSample)
                {
                case 1:
                {
                    uint64 data = 0;
                    for (uint c = 0; c < channelSize; c += bytesPerSample)
                    {
                        data += static_cast<uint64>(*(inputData + i + c));
                    }
                    monoData[j] = data / channels;
                    j += bytesPerSample;
                }
                break;
                case 2:
                {
                    int64 data = 0;
                    for (uint c = 0; c < channelSize; c += bytesPerSample)
                    {
                        data += static_cast<const int64>(*reinterpret_cast<const int16*>(inputData + i + c));
                    }
                    *reinterpret_cast<int16*>(monoData + j) = data / channels;
                    j += bytesPerSample;
                }
                break;
                case 4:
                {
                    float data = 0;
                    for (uint c = 0; c < channelSize; c += bytesPerSample)
                    {
                        data += *reinterpret_cast<const float*>(inputData + i + c);
                    }
                    *reinterpret_cast<float*>(monoData + j) = data / channels;
                    j += bytesPerSample;
                }
                break;
                default:
                    break;
                }
            }
        }

        byte* convertToMono(const byte* inputData, int dataSize, int& monoSize, int& channels, int bitsPerSample)
        {
            monoSize = dataSize / channels;
            byte* monoData = new byte[monoSize];
            if (channels == 1)
            {
                memcpy(monoData, inputData, monoSize);
                return monoData;
            }
            convertToMono(inputData, dataSize, monoData, channels, bitsPerSample);
            channels = 1;
            return monoData;
        }

        channel_data extractChannels(const byte* inputData, int dataSize, int channels, int bitsPerSamples)
        {
            assert_msg("0 was passed for channels", channels != 0);
            // channelData is a 2D array of [channels][channelData]
            // channelData will hold the audio data per channel
            channel_data channelData(channels);
            channelData.dataPerChannel.resize(channels);
            if (channels == 1)
            {
                channelData.dataPerChannel[0].resize(dataSize);
                std::copy(inputData, inputData + dataSize, std::back_inserter(channelData.dataPerChannel[0]));
                //memcpy(channelData.dataPerChannel[0], inputData, dataSize);
                return channelData;
            }

            int bytesPerSample = bitsPerSamples / 8;

            for (size_type c = 0; c < channels; ++c)
            {
                channelData.dataPerChannel[c].resize(dataSize/channels);
            }

            switch (bytesPerSample)
            {
            case 1:
            {
                uint j = 0;
                for (size_type i = 0; i < dataSize; i += bytesPerSample * channels)
                {
                    for (size_type c = 0; c < channels; ++c)
                    {
                        channelData.dataPerChannel[c][j] = inputData[i + c];
                    }
                    ++j;
                }
            }
                break;
            case 2:
            {
                uint j = 0;
                int16 data = 0;
                for (size_type i = 0; i < dataSize; i += bytesPerSample * channels)
                {
                    for (size_type c = 0; c < channels; ++c)
                    {
                        data = *reinterpret_cast<const int16*>(inputData + i + (c*bytesPerSample));
                        *reinterpret_cast<int16*>(channelData.dataPerChannel[c].data()+j) = data;
                    }
                    j+=bytesPerSample;
                }
            }
                break;
            case 4:
            {
                uint j = 0;
                float data = 0;
                for (size_type i = 0; i < dataSize; i += bytesPerSample * channels)
                {
                    for (size_type c = 0; c < channels; ++c)
                    {
                        data = *reinterpret_cast<const float*>(inputData + i + (c * bytesPerSample));
                        *reinterpret_cast<float*>(channelData.dataPerChannel[c].data() + j) = data;
                    }
                    j += bytesPerSample;
                }
            }
                break;
            default:
                break;
            }
            return channelData;
        }

        ALenum getAudioFormat(int channels, int bitsPerSample)
        {
            if (channels == 1)
            {
                if (bitsPerSample == 8) return AL_FORMAT_MONO8;
                else if (bitsPerSample == 16) return AL_FORMAT_MONO16;
                else if (bitsPerSample == 32) return AL_FORMAT_MONO_FLOAT32;
            }
            else if (channels == 2)
            {
                if (bitsPerSample == 8) return AL_FORMAT_STEREO8;
                else if (bitsPerSample == 16) return AL_FORMAT_STEREO16;
                else if (bitsPerSample == 32) return AL_FORMAT_STEREO_FLOAT32;
            }
            else if (channels == 4)
            {
                if (bitsPerSample == 8) return AL_FORMAT_QUAD8;
                else if (bitsPerSample == 16) return AL_FORMAT_QUAD16;
                else if (bitsPerSample == 32) return AL_FORMAT_QUAD32;
            }
            return AL_FORMAT_STEREO16;
        }

        void createAndBufferAudioData(ALuint* bufferId, int channels, int bitsPerSample, byte* data, int dataSize, int sampleRate)
        {
            std::lock_guard guard(AudioSystem::contextLock);
            alcMakeContextCurrent(AudioSystem::alcContext);
            //Generate openal buffer
            alGenBuffers((ALuint)1, bufferId);

            alBufferData(*bufferId, detail::getAudioFormat(channels, bitsPerSample), data, dataSize, sampleRate);

            alcMakeContextCurrent(nullptr);
        }
    }
}
