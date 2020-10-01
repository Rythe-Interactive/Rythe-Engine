#include <audio/data/importers/audio_importers.hpp>

namespace args::audio
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
            return decay(Err(args_fs_error("Failed to load audio file")));
        }

        audio_segment as{
            fileInfo.buffer,
            fileInfo.samples,
            fileInfo.channels,
            fileInfo.hz,
            fileInfo.layer,
            fileInfo.avg_bitrate_kbps
        };

        return decay(Ok(as));
    }
}
