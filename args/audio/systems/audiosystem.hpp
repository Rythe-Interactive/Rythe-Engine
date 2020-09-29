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
        virtual void setup()
        {
            data::m_alDevice = alcOpenDevice(NULL);
            if (!data::m_alDevice)
            {
                // Failed to create alcDevice
                log::error("OpenAl device failed to open");
#if defined(AUDIO_EXIT_ON_FAIL)
                raiseEvent<events::exit>();
#endif
                return;
            }

            // Succesfully created alcDevice
            log::info("Succesfully created openAl device");

            data::m_alContext = alcCreateContext(data::m_alDevice, NULL);
            if (!alcMakeContextCurrent(data::m_alContext))
            {
                // Failed to create alcContext
                log::error("OpenAl context failed to create");
#if defined(AUDIO_EXIT_ON_FAIL)
                raiseEvent<events::exit>();
#endif
                return;
            }
            // Succesfully created alcContext
            log::info("Succesfully created openAl context");

            alListener3f(AL_POSITION, 0, 0, 0);
            alListener3f(AL_VELOCITY, 0, 0, 0);
            ALfloat ori[] = { 0, 0, 1.0f, 0, 1.0f, 0 };
            alListenerfv(AL_ORIENTATION, ori);

            ALCint srate;
            alcGetIntegerv(data::m_alDevice, ALC_FREQUENCY, 1, &srate);
            log::info("OpenAl device freq: {}", srate);

            //ARGS function binding

            sourceQuery = createQuery<audio_source, position>();
            listenerQuery = createQuery<audio_listener, position, rotation>();

            createProcess<&AudioSystem::update>("Audio");
            bindToEvent<events::component_creation<audio_source>, &AudioSystem::onComponentCreate>();
            //bindToEvent<events::component_creation<audio_source>, &AudioSystem::onComponentCreate>();
        }

        void onComponentCreate(events::component_creation<audio_source>* event)
        {
            auto handle = event->entity.get_component_handle<audio_source>();
            audio_source a = handle.read();
            // do something with a.
            if (!initSource(a))
            {
                handle.destroy();
                
#if defined(AUDIO_EXIT_ON_FAIL)
                raiseEvent<events::exit>();
#endif
                return;
            }
            log::debug("playing sound");
            alSourcePlay(a.m_sourceId);

            handle.write(a);
            ++data::m_sourceCount;
        }

        void onComponentDestroy(events::component_destruction<audio_source>* event)
        {
            auto handle = event->entity.get_component_handle<audio_source>();
            audio_source a = handle.read();
            handle.write(a);
            --data::m_sourceCount;
        }

        void update(time::span deltatime);

    private:
        bool initSource(audio_source& source);

        struct ARGS_API data
        {
            static ALCdevice* m_alDevice;
            static ALCcontext* m_alContext;
            static unsigned int m_sourceCount;
        };
    };
}

#include <audio/systems/audiosystem.inl>
