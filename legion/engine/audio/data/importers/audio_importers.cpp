#include <audio/data/importers/audio_importers.hpp>
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>
#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    ALCcontext* mp3_audio_loader::context = nullptr;

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

        audio_segment as{
            fileInfo.buffer,
            0,
            fileInfo.samples,
            fileInfo.channels,
            fileInfo.hz,
            fileInfo.layer,
            fileInfo.avg_bitrate_kbps
        };

        async::readwrite_guard guard(AudioSystem::contextLock);
        alcMakeContextCurrent(context);
        //Generate openal buffer
        alGenBuffers((ALuint)1, &as.audioBufferId);
        alBufferData(as.audioBufferId, AL_FORMAT_MONO16, as.data, as.samples * sizeof(int16), as.sampleRate);
        alcMakeContextCurrent(nullptr);

        return decay(Ok(as));
    }
}
