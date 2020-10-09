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
        enum sound_properties
        {
            pitch = 1,
            gain = 2,
            playState = 4,
        };

        enum playstate
        {
            playing,
            paused,
            stopped,
        };

        /**
        * @brief Function to set the pitch for the audio source
        * @param const float pitch: new pitch value
        */
        void setPitch(const float pitch)
        {
            m_changes |= sound_properties::pitch; // set b0
            m_pitch = args::math::max(0.0f, pitch);
        }
        /**
        * @brief Function to get the current pitch
        */
        float getPitch() const { return m_pitch; }
        /**
        * @brief Function to set the gain for the audio source
        * @param const float gain: new gain value
        */
        void setGain(const float gain)
        {
            m_changes |= sound_properties::gain; // set b1
            m_gain = args::math::max(0.0f, gain);
        }
        /**
        * @brief Function to get the current gain
        */
        float getGain() const { return m_gain; };

        void play()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::playing) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::playing;
            // Do not set playstate to playing - audiosystem will set it accordingly
        }

        void pause()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::paused) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::paused;
            // Do not set playstate to paused - audiosystem will set it accordingly
        }

        void stop()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::stopped) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::stopped;
            // Do not set playstate to stopped - audiosystem will set it accordingly
        }

        bool isPlaying() const
        {
            return m_playState == playstate::playing;
        }

        bool isPaused() const
        {
            return m_playState == playstate::paused;
        }

        bool isStopped() const
        {
            return m_playState == playstate::stopped;
        }

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
        void clearChanges()
        {
            m_changes ^= m_changes; // Reset
        }

        ALuint m_sourceId;
        audio_segment_handle m_audio_handle;

        float m_pitch = 1.0f;
        float m_gain = 1.0f;

        playstate m_playState = playstate::stopped;
        playstate m_nextPlayState = playstate::stopped;

        // Byte to keep track of changes made to audio source
        // b0 - pitch
        // b1 - gain
        // b2 - play state
        // b3 - pause state
        // b4 - stop state
        byte m_changes = 0;
    };
}
