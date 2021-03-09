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

        audio_segment(byte* data, ALuint bufferId, size_type samples, int channels, int sampleRate, int layer, int avg_bitRate);

        audio_segment(const audio_segment& other);

        audio_segment(audio_segment&& other);

        audio_segment& operator=(const audio_segment& other);

        audio_segment& operator=(audio_segment&& other);

        ~audio_segment();

        // Read-Write
        byte* getData()
        {
            return m_data;
        }

        // Read only
        const byte* getData() const
        {
            return m_data;
        }

        void setNextAudioSegment(audio_segment& next)
        {
            m_next = &next;
        }

        audio_segment* getNextAudioSegment()
        {
            return m_next;
        }

        audio_segment* clearNextAudioSegment()
        {
            audio_segment* next = m_next;
            m_next = nullptr;
            return next;
        }

    private:
        static std::unordered_map<id_type, uint> m_refs;
        static std::mutex m_refsLock;
        static id_type m_lastId;
        id_type m_id;
        byte* m_data;

        audio_segment* m_next = nullptr;
    };

    /**
    * @brief Import settings for audio files
    * @brief Settings:
    * @brief force_mono: when enabled the loaded audio file will combine channels to make the audio file mono, which allows for spatial audio
    * @brief split_channels: when enabled the channels of the audio file will be loaded into seperate audio segments
    */
    struct audio_import_settings
    {
        enum struct channel_processing_setting : int
        {
            none = 0,
            force_mono,
            split_channels,
        } channel_processing;
    };

    const audio_import_settings default_audio_import_settings{ audio_import_settings::channel_processing_setting::none };

    struct audio_segment_handle
    {
        id_type id;
        std::pair<async::rw_spinlock&, audio_segment&> get() const;

        operator id_type () { return id; }
    };

    const audio_segment_handle invalid_audio_segment_handle{ invalid_id };

    class AudioSegmentCache
    {
        friend struct audio_segment_handle;
    public:
        static audio_segment_handle createAudioSegment(const std::string& name, const fs::view& file, audio_import_settings settings = default_audio_import_settings);
        static audio_segment_handle getAudioSegment(const std::string& name);
        static void unload();
    private:
        static void createAudioSegment(const std::string& name, audio_segment* segment);

        // Unorderer map to store all unique audio segments
        // Each audio segment has a unique id using name hash
        // Each segment also needs a rw_spinlock for thread safety
        static std::unordered_map<id_type, std::unique_ptr<std::pair<async::rw_spinlock, audio_segment>>> m_segments;

        static async::rw_spinlock m_segmentsLock;
    };
}
