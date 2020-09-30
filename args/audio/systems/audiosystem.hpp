#pragma once
#include <audio/components/audio_source.hpp>
#include <audio/components/audio_listener.hpp>

/**
 * @file audiosystem.hpp
 */

void openal_error()
{
    ALCenum error;
    error = alGetError();
    if (error != AL_NO_ERROR)
        args::log::warn("ERROR: OpenAl error: {}", error);
}

namespace args::audio
{
    /**@class AudioSystem
     * @brief This is a system that handles audio components.
     */
    class AudioSystem final : public System<AudioSystem>
    {
    public:

        ecs::EntityQuery sourceQuery;
        ecs::EntityQuery listenerQuery;


        ~AudioSystem()
        {

        }

        /**@brief Function for initializing the system.
         */
        virtual void setup();

        /**
        * @brief Function callback for audio_source component creation.
        * @brief Initiallizes sound source and sound file
        */
        void onComponentCreate(events::component_creation<audio_source>* event);

        /**
        * @brief Function callback for audio_source component destruction.
        */
        // NOTE TO SELF
        // Make sure this releases the audio file and source
        void onComponentDestroy(events::component_destruction<audio_source>* event);

        void update(time::span deltatime);

        static void setDistanceModel(ALenum distanceModel);

    private:
        bool initSource(audio_source& source);
        async::readonly_rw_spinlock* m_lock;

        /**
        * @brief Function to print information about openal.
        * @brief Information that will be printed includes:
        * @brief Vendor, Version, Renderer, OpenALExtensions,
        * @brief ALC Extentions and device sample rate
        */
        void queryInformation();

        struct ARGS_API data
        {
            static ALCdevice* alDevice;
            static ALCcontext* alContext;
            static unsigned int sourceCount;
        };
    };
}

#include <audio/systems/audiosystem.inl>
