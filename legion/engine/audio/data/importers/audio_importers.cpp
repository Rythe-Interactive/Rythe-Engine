#include <audio/data/importers/audio_importers.hpp>
#if !defined(DOXY_EXCLUDE)
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>
#endif
#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    common::result_decay_more<audio_segment, fs_error> mp3_audio_loader::load(const fs::basic_resource& resource, audio_import_settings&& settings)
    {
        using common::Err, common::Ok;
        using decay = common::result_decay_more<audio_segment, fs_error>;

        mp3dec_map_info_t map_info;
        map_info.buffer = resource.data();
        map_info.size = resource.size();

        mp3dec_t mp3dec;
        mp3dec_file_info_t fileInfo;

        if (mp3dec_load_mapinfo(&mp3dec, &map_info, &fileInfo, NULL, NULL))
        {
            return decay(Err(legion_fs_error("Failed to load audio file")));
        }

        byte* audioData;

        // bitsPerSample is always 16 for mp3
        int dataSize = fileInfo.samples * sizeof(int16);
        int channels = fileInfo.channels;
        int samples = fileInfo.samples;

        if (settings.force_mono)
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

        async::readwrite_guard guard(AudioSystem::contextLock);
        alcMakeContextCurrent(AudioSystem::alcContext);
        //Generate openal buffer
        alGenBuffers((ALuint)1, &as.audioBufferId);

        ALenum format = AL_FORMAT_MONO16;
        if (as.channels == 2) format = AL_FORMAT_STEREO16;

        alBufferData(as.audioBufferId, format, as.getData(), dataSize, as.sampleRate);

        alcMakeContextCurrent(nullptr);

        return decay(Ok(as));
    }

    common::result_decay_more<audio_segment, fs_error> wav_audio_loader::load(const fs::basic_resource& resource, audio_import_settings&& settings)
    {
        using common::Err, common::Ok;
        using decay = common::result_decay_more<audio_segment, fs_error>;

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
            return decay(Err(legion_fs_error("WAV File invalid header, exptected RIFF")));
        }

        if (header.format[0] != 'W' ||
            header.format[1] != 'A' ||
            header.format[2] != 'V' ||
            header.format[3] != 'E')
        {
            log::error("Found WAV format: '{}{}{}{}', exptected: 'WAVE'", (char)header.format[0], (char)header.format[1], (char)header.format[2], (char)header.format[3]);
            return decay(Err(legion_fs_error("Loaded File is not of type WAV")));
        }

        if (header.wave_format.subChunckId[0] != 'f' ||
            header.wave_format.subChunckId[1] != 'm' ||
            header.wave_format.subChunckId[2] != 't' ||
            header.wave_format.subChunckId[3] != ' ')
        {
            log::error("Found WAV format sub chunck ID: '{}{}{}{}', exptected: 'fmt '", (char)header.wave_format.subChunckId[0], (char)header.wave_format.subChunckId[1], (char)header.wave_format.subChunckId[2], (char)header.wave_format.subChunckId[3]);
            return decay(Err(legion_fs_error("WAV File sub chunck id was not (fmt )")));
        }

        memcpy(&waveData, resource.data() + sizeof(header), sizeof(waveData));
        if (waveData.subChunckId[0] != 'd' ||
            waveData.subChunckId[1] != 'a' ||
            waveData.subChunckId[2] != 't' ||
            waveData.subChunckId[3] != 'a')
        {
            log::error("Found WAV data sub chunck ID: '{}{}{}{}', exptected: 'data'", (char)waveData.subChunckId[0], (char)waveData.subChunckId[1], (char)waveData.subChunckId[2], (char)waveData.subChunckId[3]);
            return decay(Err(legion_fs_error("WAV File sample data does not start with word (data)")));
        }

        uint metaSize = sizeof(header) + sizeof(waveData);

        int sampleDataSize = resource.size() - metaSize;
        byte* audioData;

        int channels = header.wave_format.channels;

        if (settings.force_mono)
        {
            audioData = detail::convertToMono(resource.data() + metaSize, sampleDataSize, sampleDataSize, channels, header.wave_format.bitsPerSample);
        }
        else
        {
            // Leave as is
            audioData = new byte[sampleDataSize];
            memcpy(audioData, resource.data() + metaSize, sampleDataSize);
        }

        audio_segment as(
            audioData,
            0,
            sampleDataSize / (header.wave_format.bitsPerSample/8), // Sample count, unknown for wav
            channels,
            (int)header.wave_format.sampleRate,
            -1, // Layer, does not exist in wav
            -1 // avg_biterate_kbps, unknown for wav
        );

        async::readwrite_guard guard(AudioSystem::contextLock);
        alcMakeContextCurrent(AudioSystem::alcContext);
        //Generate openal buffer
        alGenBuffers((ALuint)1, &as.audioBufferId);
        ALenum format = AL_FORMAT_MONO16;
        if (as.channels == 1)
        {
            if (header.wave_format.bitsPerSample == 8) format = AL_FORMAT_MONO8;
            else if (header.wave_format.bitsPerSample == 16) format = AL_FORMAT_MONO16;
            else if (header.wave_format.bitsPerSample == 32) format = AL_FORMAT_MONO_FLOAT32;
        }
        else if (as.channels == 2)
        {
            if (header.wave_format.bitsPerSample == 8) format = AL_FORMAT_STEREO8;
            else if (header.wave_format.bitsPerSample == 16) format = AL_FORMAT_STEREO16;
            else if (header.wave_format.bitsPerSample == 32) format = AL_FORMAT_STEREO_FLOAT32;
        }
        else if (as.channels == 4)
        {
            if (header.wave_format.bitsPerSample == 8) format = AL_FORMAT_QUAD8;
            else if (header.wave_format.bitsPerSample == 16) format = AL_FORMAT_QUAD16;
            else if (header.wave_format.bitsPerSample == 32) format = AL_FORMAT_QUAD32;
        }
        alBufferData(as.audioBufferId, format, as.getData(), sampleDataSize, as.sampleRate);

        alcMakeContextCurrent(nullptr);

        return decay(Ok(as));
    }

    namespace detail
    {
        void convertToMono(const byte* inputData, int dataSize, byte* monoData, int channels, int bitsPerSample)
        {
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

    }
}
