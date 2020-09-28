#pragma once
#include <audio/detail/engine_include.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#define MINIMP3_IMPLEMENTATION
#include <minimp3.h>
#include <minimp3_ex.h>

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
        const float getPitch() { return m_pitch; };
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
        const float getGain() { return m_gain; };

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
        mp3dec_t m_mp3dec;
        mp3dec_file_info_t m_audioInfo;

        float m_pitch;
        float m_gain;

        // Byte to keep track of changes made to audio source
        // b0 - pitch changed
        // b1 - gain changed
        byte m_changes;
    };
}
