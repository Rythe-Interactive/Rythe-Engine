#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <audio/data/audio_segment.hpp>

namespace args::audio
{
    struct audio_source
    {
        friend class AudioSystem;
    public:
        /**
        * @brief Function to set the pitch for the audio source
        * @param const float pitch: new pitch value
        */
        float setPitch(const float pitch)
        {
            m_changes |= 1; // set b0
            m_pitch = pitch;
        }
        /**
        * @brief Function to get the current pitch
        */
        float getPitch() const { return m_pitch; }
        /**
        * @brief Function to set the gain for the audio source
        * @param const float gain: new gain value
        */
        float setGain(const float gain)
        {
            m_changes |= 2; // set b1
            m_gain = gain;
        }
        /**
        * @brief Function to get the current gain
        */
        float getGain() const { return m_gain; };

        void setAudioHandle(audio_segment_handle handle)
        {
            m_audio_handle = handle;
        }

        audio_segment_handle getAudioHandle() const
        {
            return m_audio_handle;
        }

    private:
        /**
        * @brief Function to clear the changes that will be applied
        */
        const void clearChanges()
        {
            m_changes = 0;
        }

        ALuint m_sourceId;
        ALuint m_audioBufferId;
        audio_segment_handle m_audio_handle;

        float m_pitch;
        float m_gain;

        // Byte to keep track of changes made to audio source
        // b0 - pitch changed
        // b1 - gain changed
        byte m_changes;
    };
}
