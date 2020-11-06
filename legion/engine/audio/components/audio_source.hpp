#pragma once
#if !defined(DOXY_EXCLUDE)
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include <audio/data/audio_segment.hpp>

namespace legion::audio
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
            doRewind = 8,
            audioHandle = 16,
            rollOffFactor = 32,
        };

        enum playstate
        {
            stopped,
            playing,
            paused,
        };

        /**
        * @brief Function to set the pitch for the audio source
        * @param const float pitch: new pitch value
        */
        void setPitch(const float pitch)
        {
            m_changes |= sound_properties::pitch; // set b0
            m_pitch = legion::math::max(0.0f, pitch);
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
            m_gain = legion::math::max(0.0f, gain);
        }
        /**
        * @brief Function to get the current gain
        */
        float getGain() const { return m_gain; };

        /**
        * @brief Function to set the roll off factor for 3D audio
        * @brief Rolloff factor only works for mono audio
        */
        void setRollOffFactor(float factor)
        {
            m_changes = sound_properties::rollOffFactor;
            m_rolloffFactor = factor;
        }

        /**
        * @brief Function to disable spatial (3D) 
        * @brief Calls setRtollOffFactor(0.0f)
        */
        void disableSpatialAudio()
        {
            setRollOffFactor(0.0f);
        }

        /**
        * @brief Function to enable spatial (3D)
        * @brief Calls setRtollOffFactor(1.0f)
        */
        void enableSpatialAudio()
        {
            setRollOffFactor(1.0f);
        }

        /**
        * @brief Plays audio 
        */
        void play()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::playing) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::playing;
            // Do not set playstate to playing - audiosystem will set it accordingly
        }

        /**
        * @brief Pauses audio
        */
        void pause()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::paused) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::paused;
            // Do not set playstate to paused - audiosystem will set it accordingly
        }

        /**
        * @brief Stops audio
        * @brief Stopping means that the audio will stop playing (pausing) and rewind
        */
        void stop()
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::stopped) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::stopped;
            // Do not set playstate to stopped - audiosystem will set it accordingly
        }

        /**
        * @brief Returns whether the audio is playing
        * @brief If false the audio can be paused (isPaused()) or stopped (isStopped())
        */
        bool isPlaying() const
        {
            return m_playState == playstate::playing;
        }

        /**
        * @brief Returns whether the audio is paused
        * @brief If false the audio can be playing (isPlaying()) or stopped (isStopped())
        */
        bool isPaused() const
        {
            return m_playState == playstate::paused;
        }

        /**
        * @brief Returns whether the audio is stopped
        * @brief If false the audio can be playing (isPlaying()) or paused (isPaused())
        */
        bool isStopped() const
        {
            return m_playState == playstate::stopped;
        }

        void setAudioHandle(audio_segment_handle handle)
        {
            if (handle == m_audio_handle) return;
            m_changes |= sound_properties::audioHandle;
            m_audio_handle = handle;
        }

        /**
        * @brief Rewinds the audio
        * @brief If the audio is playing it will stop/pause
        * @brief If the audio was playing the audio source needs to be stopped or paused before it can play
        */
        void rewind()
        {
            m_changes |= sound_properties::doRewind;
        }

        audio_segment_handle getAudioHandle() const
        {
            return m_audio_handle;
        }

        int getChannels()
        {
            int channels = 0;
            {
                async::readonly_guard guard(m_audio_handle.get().first);
                channels = m_audio_handle.get().second.channels;
            }
            return channels;
        }

        bool isStereo()
        {
            return getChannels() == 2;
        }

        bool isMono()
        {
            return getChannels() == 1;
        }

    private:
        /**
        * @brief Function to clear the changes that will be applied
        */
        void clearChanges()
        {
            m_changes ^= m_changes; // Reset
            // The next play state also needs to be reset to be able to properly switch play states
            m_nextPlayState = m_playState; 
        }

        ALuint m_sourceId;
        audio_segment_handle m_audio_handle = invalid_audio_segment_handle;

        float m_pitch = 1.0f;
        float m_gain = 1.0f;

        playstate m_playState = playstate::stopped;
        playstate m_nextPlayState = playstate::stopped;

        float m_rolloffFactor;

        // Byte to keep track of changes made to audio source
        // For all the values > see enum sound_properties
        // b0 - pitch
        // b1 - gain
        // b2 - play state
        // b3 - rewind (doRewind)
        // b4 - audio handle
        // b5 - roll off factor 3D
        byte m_changes = 0;
    };
}
