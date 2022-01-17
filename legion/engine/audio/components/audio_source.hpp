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
        constexpr static ALuint invalid_source_id = -1;

        enum sound_properties
        {
            pitch = 1 << 0,
            gain = 1 << 1,
            playState = 1 << 2,
            doRewind = 1 << 3,
            audioHandle = 1 << 4,
            rollOffFactor = 1 << 5,
            looping = 1 << 6,
            rollOffDistance = 1 << 7
        };

        enum playstate
        {
            stopped,
            playing,
            paused,
        };

        audio_source() = default;

        audio_source(const audio_segment_handle& handle)
        {
            m_changes |= sound_properties::audioHandle;
            m_audio_handle = handle;
        }

        /**
         * @brief Function to set the pitch for the audio source
         * @param pitch new pitch value
         */
        void setPitch(float pitch) noexcept
        {
            m_changes |= sound_properties::pitch; // set b0
            m_pitch = legion::math::max(0.0f, pitch);
        }
        /**
         * @brief Function to get the current pitch
         */
        float getPitch() const noexcept { return m_pitch; }

        /**
         * @brief Function to set the pitch for the audio source
         * @param pitch new pitch value
         */
        void setRollOffDistances(float refDist, float maxDist) noexcept
        {
            m_changes |= sound_properties::rollOffDistance; // set b0
            m_referenceDistance = math::max(0.0f, refDist);
            m_maxDistance = math::max(0.0f, maxDist);
        }

        /**
         * @brief Function to get the current pitch
         */
        float getReferenceDistance() const noexcept { return m_referenceDistance; }
        float getMaxDistance() const noexcept { return m_maxDistance; }

        /**
         * @brief Function to set the gain for the audio source
         * @param gain new gain value
         */
        void setGain(float gain) noexcept
        {
            m_changes |= sound_properties::gain; // set b1
            m_gain = legion::math::max(0.0f, gain);
        }
        /**
         * @brief Function to get the current gain
         */
        float getGain() const noexcept { return m_gain; }

        /**
         * @brief Function to set the roll off factor for 3D audio
         * @param factor only works for mono audio
         */
        void setRollOffFactor(float factor) noexcept
        {
            m_changes = sound_properties::rollOffFactor;
            m_rolloffFactor = factor;
        }

        /**
         * @brief Function to disable spatial (3D)
         * Calls setRollOffFactor(0.0f)
         */
        void disableSpatialAudio() noexcept
        {
            setRollOffFactor(0.0f);
        }

        /**
         * @brief Function to enable spatial (3D)
         * Calls setRollOffFactor(1.0f)
         */
        void enableSpatialAudio() noexcept
        {
            setRollOffFactor(1.0f);
        }

        /**
         * @brief Plays audio
         */
        void play() noexcept
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
        void pause() noexcept
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::paused) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::paused;
            // Do not set playstate to paused - audiosystem will set it accordingly
        }

        /**
         * @brief Stops audio
         * Stopping means that the audio will stop playing (pausing) and rewind
         */
        void stop() noexcept
        {
            // If the file is already playing or if the file will be played on next update > return
            if (m_nextPlayState == playstate::stopped) return;
            m_changes |= sound_properties::playState;
            m_nextPlayState = playstate::stopped;
            // Do not set playstate to stopped - audiosystem will set it accordingly
        }

        /**
         * @brief Returns whether the audio is playing
         * If false the audio can be paused (isPaused()) or stopped (isStopped())
         */
        bool isPlaying() const noexcept
        {
            return m_playState == playstate::playing;
        }

        /**
         * @brief Returns whether the audio is paused
         * If false the audio can be playing (isPlaying()) or stopped (isStopped())
         */
        bool isPaused() const noexcept
        {
            return m_playState == playstate::paused;
        }

        /**
         * @brief Returns whether the audio is stopped
         *  If false the audio can be playing (isPlaying()) or paused (isPaused())
         */
        bool isStopped() const noexcept
        {
            return m_playState == playstate::stopped;
        }

        void setAudioHandle(audio_segment_handle handle) noexcept
        {
            if (handle == m_audio_handle) return;
            m_changes |= sound_properties::audioHandle;
            m_audio_handle = handle;
        }

        /**
         * @brief Rewinds the audio
         * If the audio is playing it will stop/pause
         * If the audio was playing the audio source needs to be stopped or paused before it can play
         */
        void rewind() noexcept
        {
            m_changes |= sound_properties::doRewind;
        }

        audio_segment_handle getAudioHandle() const noexcept
        {
            return m_audio_handle;
        }

        /**
         * @brief Gets the amount of channels in this audio source
         */
        int getChannels() const
        {
            async::readonly_guard guard(m_audio_handle.get().first);
            return m_audio_handle.get().second.channels;
        }

        /**
         * @brief Returns whether the audio-source is stereo (channels=2)
         */
        bool isStereo() const noexcept
        {
            return getChannels() == 2;
        }
        /**
         * @brief Returns whether the audio-source is mono (channels=1)
         */
        bool isMono() const noexcept
        {
            return getChannels() == 1;
        }

        /**
         * @brief Sets the looping state of the audio-source (true=looping)
         */
        void setLooping(bool state = false) noexcept
        {
            if (state != m_looping) {
                m_looping = state;
                m_changes |= looping;
            }
        }

        /**
         * @brief Gets the looping state of the audio-source (true=looping)
         */
        bool isLooping() const noexcept
        {
            return m_looping;
        }


        /**
         * @brief Helper to implicitly convert to OpenAL source
         */
        operator ALuint() const
        {
            return m_sourceId;
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
        float m_referenceDistance = 5.f;
        float m_maxDistance = 15.f;

        bool m_looping = false;

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
