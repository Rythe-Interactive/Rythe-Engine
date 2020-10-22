#pragma once
#include <core/core.hpp>
#include <mutex>
#if !defined(DOXY_EXCLUDE)
#include <AL/al.h>
#include <AL/alc.h>
#endif
namespace legion::audio
{
	struct audio_segment
	{
	public:
        ALuint audioBufferId;

        /* Channels included, byte size = samples*sizeof(int16) */
        size_type samples; 
        int channels, sampleRate, layer, avg_bitrate_kbps;

        audio_segment() = default;

        audio_segment(int16* data, ALuint bufferId, size_type samples, int channels, int sampleRate, int layer, int avg_bitRate);

        audio_segment(const audio_segment& other);

        audio_segment(audio_segment&& other);

        audio_segment& operator=(const audio_segment& other);

        audio_segment& operator=(audio_segment&& other);

        ~audio_segment();

        // Read-Write
        int16* getData()
        {
            return m_data;
        }

        // Read only
        const int16* getData() const
        {
            return m_data;
        }

    private:
        static std::unordered_map<id_type, uint> m_refs;
        static std::mutex m_refsLock;
        static id_type m_lastId;
        id_type m_id;
        int16* m_data;
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

    class AudioSegmentCache
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
