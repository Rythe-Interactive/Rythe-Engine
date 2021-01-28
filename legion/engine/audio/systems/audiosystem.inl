#pragma once
#include <core/scenemanagement/scenemanager.hpp>

namespace legion::audio
{
    inline void AudioSystem::setup()
    {
        alDevice = alcOpenDevice(NULL);
        if (!alDevice)
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

        alcContext = alcCreateContext(alDevice, NULL);
        if (!alcMakeContextCurrent(alcContext))
        {
            // Failed to create alcContext
            log::error("OpenAl context failed to create");
            openal_error();
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
        m_listenerPosition = position(0, 0, 0);
        log::info("initialized listener!");

        alDopplerFactor(1.0f);
        alDistanceModel(AL_EXPONENT_DISTANCE);

        queryInformation();

        //m_sourcePositions = std::unordered_map<ecs::component_handle<audio_source>, position>();

        //ARGS function binding

        sourceQuery = createQuery<audio_source>();

        createProcess<&AudioSystem::update>("Update");
        bindToEvent<events::component_creation<audio_source>, &AudioSystem::onAudioSourceComponentCreate>();
        bindToEvent<events::component_destruction<audio_source>, &AudioSystem::onAudioSourceComponentDestroy>();
        bindToEvent<events::component_creation<audio_listener>, &AudioSystem::onAudioListenerComponentCreate>();
        bindToEvent<events::component_destruction<audio_listener>, &AudioSystem::onAudioListenerComponentDestroy>();
        bindToEvent<events::exit, &AudioSystem::onEngineExit>();

        // Release context on this thread
        alcMakeContextCurrent(nullptr);
    }

    inline void AudioSystem::onEngineExit(events::exit* event)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        sourceQuery.queryEntities();
        for (auto entity : sourceQuery)
        {
            auto sourceHandle = entity.get_component_handle<audio_source>();
            sourceHandle.destroy();
        }

        alDevice = alcGetContextsDevice(alcContext);
        alcMakeContextCurrent(nullptr);
        AudioSegmentCache::unload();
        alcDestroyContext(alcContext);
        alcCloseDevice(alDevice);
    }

    inline AudioSystem::~AudioSystem()
    {
    }

    inline void AudioSystem::queryInformation()
    {
        // This function is called from setup()
        // In setup everything is still single threaded and therefore no lock is required

        const ALchar* vendor = alGetString(AL_VENDOR);
        const ALchar* version = alGetString(AL_VERSION);
        const ALchar* renderer = alGetString(AL_RENDERER);
        const ALchar* openALExtensions = alGetString(AL_EXTENSIONS);
        const ALchar* ALCExtensions = alcGetString(alDevice, ALC_EXTENSIONS);
        ALCint srate;
        alcGetIntegerv(alDevice, ALC_FREQUENCY, 1, &srate);
        log::info(
            "Initialized OpenAL\n\tCONTEXT INFO\n\t"\
            "----------------------------------\n\t"\
            "Vendor:\t\t\t{}\n\tVersion:\t\t{}\n\t"\
            "Renderer:\t\t{}\n\tDevice samplerate:\t{}\n\t"\
            "OpenAl Extensions:\n\t\t{}\n\n\t"\
            "ALC Extensions:\n\t\t{}\n\t"\
            "----------------------------------\n",
            vendor, version, renderer, srate, openALExtensions, ALCExtensions);
    }

    inline void AudioSystem::update(time::span deltatime)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        sourceQuery.queryEntities();
        for (auto entity : sourceQuery)
        {
            auto sourceHandle = entity.get_component_handle<audio_source>();

            audio_source source = sourceHandle.read();
            position p = entity.read_component<position>();
            position& previousP = m_sourcePositions.at(sourceHandle);
            math::vec3 vel = previousP - p;
            previousP = p;
            alSource3f(source.m_sourceId, AL_POSITION, p.x, p.y, p.z);
            alSource3f(source.m_sourceId, AL_VELOCITY, vel.x, vel.y, vel.z);

            using change = audio_source::sound_properties;

            if (source.m_changes & change::audioHandle)
            {
                using state = audio::audio_source::playstate;
                if (source.m_playState != state::stopped)
                {
                    alSourceStop(source.m_sourceId);
                    source.m_playState = state::stopped;
                }
                if (source.m_audio_handle) // audio has segment
                {
                    auto [segmentLock, segment] = source.m_audio_handle.get();
                    async::readwrite_guard segmentGuard(segmentLock);
                    alSourcei(source.m_sourceId, AL_BUFFER, segment.audioBufferId);
                }
                else // audio has no segment
                {
                    source.m_nextPlayState = state::stopped;
                    // Remove buffer from audio source
                    alSourcei(source.m_sourceId, AL_BUFFER, NULL);
                }
            }
            if (source.m_changes & change::pitch)
            {
                // Pitch has changed
                alSourcef(source.m_sourceId, AL_PITCH, source.getPitch());
            }
            if (source.m_changes & change::gain)
            {
                // Gain has changed
                alSourcef(source.m_sourceId, AL_GAIN, source.getGain());
            }
            if (source.m_changes & change::playState)
            {
                using state = audio::audio_source::playstate;
                if (source.m_audio_handle)
                {
                    // Playstate has changed
                    if (source.m_nextPlayState == state::playing)
                    {
                        source.m_playState = state::playing;
                        alSourcePlay(source.m_sourceId);
                    }
                    else if (source.m_nextPlayState == state::paused)
                    {
                        source.m_playState = state::paused;
                        alSourcePause(source.m_sourceId);
                    }
                    else if (source.m_nextPlayState == state::stopped)
                    {
                        source.m_playState = state::stopped;
                        alSourceStop(source.m_sourceId);
                    }
                }
                else
                {
                    source.m_nextPlayState = state::stopped;
                }
            }
            if (source.m_changes & change::doRewind)
            {
                alSourceRewind(source.m_sourceId);
            }
            if (source.m_changes & change::rollOffFactor)
            {
                alSourcef(source.m_sourceId, AL_ROLLOFF_FACTOR, source.m_rolloffFactor);
            }
            if(source.m_changes & change::looping)
            {
                alSourcei(source,AL_LOOPING ,static_cast<int>(source.m_looping));
            }


            source.clearChanges();

            ALenum isPlaying;
            alGetSourcei(source.m_sourceId,AL_SOURCE_STATE,&isPlaying);
            if(isPlaying == AL_STOPPED){
                source.m_playState = audio_source::stopped;
                source.m_nextPlayState = audio_source::stopped;
            }

            sourceHandle.write(source);

            openal_error();
        }

        if (m_listenerEnt)
        {
            position p = m_listenerEnt.read_component<position>();
            rotation r = m_listenerEnt.read_component<rotation>();

            setListener(p, r);

            math::vec3 vel = m_listenerPosition - p;
            m_listenerPosition = p;
            alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
        }

        alcMakeContextCurrent(nullptr);
    }

#pragma region Component creation&destruction
    inline void AudioSystem::onAudioSourceComponentCreate(events::component_creation<audio_source>* event)
    {
        auto handle = event->entity.get_component_handle<audio_source>();
        audio_source a = handle.read();

        // do something with a.
        initSource(a);

        m_sourcePositions.emplace(handle, event->entity.read_component<position>());

        handle.write(a);
        ++sourceCount;
    }


    inline void AudioSystem::onAudioSourceComponentDestroy(events::component_destruction<audio_source>* event)
    {
        auto handle = event->entity.get_component_handle<audio_source>();
        m_sourcePositions.erase(handle);
        audio_source a = handle.read();
        if(a.m_playState != audio_source::playstate::stopped) alSourceStop(a.m_sourceId);
        alSourcei(a.m_sourceId, AL_BUFFER, NULL);
        alDeleteSources(1, &a.m_sourceId); // Clear source
        --sourceCount;
    }

    inline void AudioSystem::onAudioListenerComponentCreate(events::component_creation<audio_listener>* event)
    {
        log::debug("Creating Audio Listener...");

        ++listenerCount;
        if (listenerCount > 1)
        {
            event->entity.remove_component<audio_listener>();
        }
        else
        {
            // listener count == 1
            m_listenerEnt = event->entity;
            setListener(event->entity.read_component<position>(), event->entity.read_component<rotation>());
            m_listenerPosition = event->entity.read_component<position>();
        }
    }

    inline void AudioSystem::onAudioListenerComponentDestroy(events::component_destruction<audio_listener>* event)
    {
        log::debug("Destroying Audio Listener...");

        listenerCount = math::max(static_cast<int>(listenerCount - 1), 0);
        if (listenerCount == 0)
        {
            log::debug("No Listeners left, resetting listener");
            //m_listenerEnt = ecs::entity_handle();
            // Reset listener
            std::lock_guard guard(contextLock);
            alcMakeContextCurrent(alcContext);
            alListener3f(AL_POSITION, 0, 0, 0);
            alListener3f(AL_VELOCITY, 0, 0, 0);
            ALfloat ori[] = { 0, 0, 1.0f, 0, 1.0f, 0 };
            alListenerfv(AL_ORIENTATION, ori);
        }
    }


#pragma endregion

    inline void AudioSystem::initSource(audio_source& source)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        alGenSources((ALuint)1, &source.m_sourceId);
        alSourcef(source.m_sourceId, AL_PITCH, 1);
        alSourcef(source.m_sourceId, AL_GAIN, 1);
        alSourcei(source.m_sourceId, AL_LOOPING, AL_FALSE);

        // 3D audio stuffs
        alSourcef(source.m_sourceId, AL_ROLLOFF_FACTOR, 1.0f);
        alSourcef(source.m_sourceId, AL_REFERENCE_DISTANCE, 5);
        alSourcef(source.m_sourceId, AL_MAX_DISTANCE, 15);

        // NOTE TO SELF:
        //		Set position and velocity to entity position and velocty
        alSource3f(source.m_sourceId, AL_POSITION, 0, 0, 0);
        alSource3f(source.m_sourceId, AL_VELOCITY, 0, 0, 0);

        if (source.m_audio_handle)
        {
            auto [segmentLock, segment] = source.m_audio_handle.get();
            async::readwrite_guard segmentGuard(segmentLock);
            alSourcei(source.m_sourceId, AL_BUFFER, segment.audioBufferId);
        }
        source.clearChanges();
        alcMakeContextCurrent(nullptr);
    }

    inline void AudioSystem::setDistanceModel(ALenum distanceModel)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);
        alDistanceModel(distanceModel);
        alcMakeContextCurrent(nullptr);
    }

    inline void AudioSystem::setListener(position p, rotation r)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);
        // Position - invert x for left-right hand coord system conversion
        alListener3f(AL_POSITION, p.x, p.y, p.z);
        //rotation
        math::mat3 mat3 = math::toMat3(r);
        // Invert z axis here for left-right hand coord system conversion
        math::vec3 forward = mat3 * math::vec3(0.f, 0.f, -1.f);
        math::vec3 up = mat3 * math::vec3(0.f, 1.f, 0.f);
        ALfloat ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, ori);
        alcMakeContextCurrent(nullptr);
    }
}
