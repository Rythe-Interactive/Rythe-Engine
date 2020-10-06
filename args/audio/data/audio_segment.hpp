#pragma once
#include <core/core.hpp>
#include <AL/al.h>
#include <AL/alc.h>

namespace args::audio
{
	struct audio_segment
	{
	public:
        int16* data;
        ALuint audioBufferId;

        /* Channels included, byte size = samples*sizeof(int16) */
        size_type samples; 
        int channels, sampleRate, layer, avg_bitrate_kbps;
	};

    struct audio_import_settings
    {
        
    };

    const audio_import_settings default_audio_import_settings{};

    struct audio_segment_handle
    {
        id_type id;
        std::pair<async::readonly_rw_spinlock&, audio_segment&> get();

        operator id_type () { return id; }
    };

    const audio_segment_handle invalid_audio_segment_handle{ invalid_id };

    class ARGS_API AudioSegmentCache
    {
        friend struct audio_segment_handle;
    public:
        static audio_segment_handle createAudioSegment(const std::string& name, const fs::view& file, audio_import_settings settings = default_audio_import_settings);
    private:
        // Unorderer map to store all unique audio segments
        // Each audio segment has a unique id using name hash
        // Each segment also needs a readonly_rw_spinlock for thread safety
        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, audio_segment>>> m_segments;

        static async::readonly_rw_spinlock m_segmentsLock;
    };
}
