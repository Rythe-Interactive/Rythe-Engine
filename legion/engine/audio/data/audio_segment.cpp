#include <audio/data/audio_segment.hpp>
#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    std::unordered_map<id_type, uint> audio_segment::m_refs;
    std::mutex audio_segment::m_refsLock;
    id_type audio_segment::m_lastId = 1;

    audio_segment::audio_segment(byte* data, ALuint bufferId, size_type samples, int channels, int sampleRate, int layer, int avg_bitRate) :
        audioBufferId(bufferId), samples(samples), channels(channels), sampleRate(sampleRate), layer(layer), avg_bitrate_kbps(avg_bitRate)
    {
        std::lock_guard guard(m_refsLock);
        m_id = m_lastId++;
        m_refs[m_id]++;
        this->m_data = data;
    }

    audio_segment::audio_segment(const audio_segment& other) :
        m_data(other.m_data), audioBufferId(other.audioBufferId), samples(other.samples), channels(other.channels), sampleRate(other.sampleRate), layer(other.layer), avg_bitrate_kbps(other.avg_bitrate_kbps), m_id(other.m_id)
    {
        if (m_id)
        {
            std::lock_guard guard(m_refsLock);
            m_refs[m_id]++;
        }
    }

    audio_segment::audio_segment(audio_segment&& other) :
        m_data(other.m_data), audioBufferId(other.audioBufferId), samples(other.samples), channels(other.channels), sampleRate(other.sampleRate), layer(other.layer), avg_bitrate_kbps(other.avg_bitrate_kbps), m_id(other.m_id)
    {
        if (m_id)
        {
            std::lock_guard guard(m_refsLock);
            m_refs[m_id]++;
        }
    }

    audio_segment& audio_segment::operator=(const audio_segment& other)
    {
        {
            std::lock_guard guard(m_refsLock);
            if (m_id)
            {
                m_refs[m_id]--;
                if (m_refs[m_id] == 0)
                {
                    {
                        async::readwrite_guard guard(AudioSystem::contextLock);
                        alcMakeContextCurrent(AudioSystem::alcContext);
                        alDeleteBuffers(1, &audioBufferId);
                        alcMakeContextCurrent(nullptr);
                    }
                    delete[] m_data;
                    m_data = nullptr;
                    m_refs.erase(m_id);
                }
            }
            m_id = other.m_id;
            m_refs[m_id]++;
        }
        m_data = other.m_data;
        audioBufferId = other.audioBufferId;
        samples = other.samples;
        channels = other.channels;
        sampleRate = other.sampleRate;
        layer = other.layer;
        avg_bitrate_kbps = other.avg_bitrate_kbps;
    }

    audio_segment& audio_segment::operator=(audio_segment&& other)
    {
        {
            std::lock_guard guard(m_refsLock);
            if (m_id)
            {
                m_refs[m_id]--;
                if (m_refs[m_id] == 0)
                {
                    {
                        async::readwrite_guard guard(AudioSystem::contextLock);
                        alcMakeContextCurrent(AudioSystem::alcContext);
                        alDeleteBuffers(1, &audioBufferId);
                        alcMakeContextCurrent(nullptr);
                    }
                    delete[] m_data;
                    m_data = nullptr;
                    m_refs.erase(m_id);
                }
            }
            m_id = other.m_id;
            m_refs[m_id]++;
        }
        m_data = other.m_data;
        audioBufferId = other.audioBufferId;
        samples = other.samples;
        channels = other.channels;
        sampleRate = other.sampleRate;
        layer = other.layer;
        avg_bitrate_kbps = other.avg_bitrate_kbps;
    }

    audio_segment::~audio_segment()
    {
        if (m_id)
        {
            std::lock_guard guard(m_refsLock);
            m_refs[m_id]--;
            if (m_refs[m_id] == 0)
            {
                {
                    async::readwrite_guard guard(AudioSystem::contextLock);
                    alcMakeContextCurrent(AudioSystem::alcContext);
                    alDeleteBuffers(1, &audioBufferId);
                    alcMakeContextCurrent(nullptr);
                }
                delete[] m_data;
                m_data = nullptr;
                m_refs.erase(m_id);
            }
        }
    }

    audio_segment_handle AudioSegmentCache::createAudioSegment(const std::string& name, const fs::view& file, audio_import_settings settings)
    {
        id_type id = nameHash(name);
        {
            async::readonly_guard guard(m_segmentsLock);
            // check if segment has been loaded before
            if (m_segments.count(id))
                return { id };
        }

        // Segment is loaded for the first time
        auto result = fs::AssetImporter::tryLoad<audio_segment>(file, settings);
        if (result != common::valid)
        {
            //log::error("Audio file wrong!");
            log::error("Error while loading file: {}, {}", static_cast<std::string>(file.get_filename()), result.get_error());
            return invalid_audio_segment_handle;
        }

        // Succesfully loaded audio segment

        {
            async::readwrite_guard guard(m_segmentsLock);
            auto* pairPointer = new std::pair<async::readonly_rw_spinlock, audio_segment>(
                std::make_pair<async::readonly_rw_spinlock, audio_segment>(async::readonly_rw_spinlock(),
                    static_cast<audio_segment>(result)));
            m_segments.emplace(std::make_pair(id, std::unique_ptr<std::pair<async::readonly_rw_spinlock, audio_segment>>(pairPointer)));
        }

        return { id };
    }

    audio_segment_handle AudioSegmentCache::getAudioSegment(const std::string& name)
    {
        id_type id = nameHash(name);
        {
            async::readonly_guard guard(m_segmentsLock);
            // check if segment has been loaded before
            if (m_segments.count(id))
                return { id };
        }
        return invalid_audio_segment_handle;
    }

    void AudioSegmentCache::unload()
    {
        async::readonly_guard guard(AudioSegmentCache::m_segmentsLock);
        m_segments.clear();
    }

    std::pair<async::readonly_rw_spinlock&, audio_segment&> audio_segment_handle::get()
    {
        async::readonly_guard guard(AudioSegmentCache::m_segmentsLock);

        auto& [lock, segment] = *(AudioSegmentCache::m_segments[id].get());

        return std::make_pair(std::ref(lock), std::ref(segment));
    }

    std::unordered_map < id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, audio_segment>>> AudioSegmentCache::m_segments;
    async::readonly_rw_spinlock AudioSegmentCache::m_segmentsLock;
}
