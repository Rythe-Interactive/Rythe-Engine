#include <audio/data/audio_segment.hpp>

namespace args::audio
{
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
                    std::move(static_cast<audio_segment>(result))));
            m_segments.emplace(std::make_pair(id, std::unique_ptr<std::pair<async::readonly_rw_spinlock, audio_segment>>(pairPointer)));
        }

        return { id };
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
