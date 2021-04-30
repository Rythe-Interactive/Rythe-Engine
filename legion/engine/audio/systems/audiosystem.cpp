#include <regex>

#include <audio/systems/audiosystem.hpp>

namespace legion::audio
{
    async::spinlock AudioSystem::contextLock;
    ALCdevice* AudioSystem::alDevice = nullptr;
    ALCcontext* AudioSystem::alcContext = nullptr;
    unsigned int AudioSystem::sourceCount = 0;
    unsigned int AudioSystem::listenerCount = 0;

    ALCint AudioSystem::frequency = 0;
    ALCint AudioSystem::refresh = 0;
    ALCint AudioSystem::sync = 0;
    ALCint AudioSystem::monoSources = 0;
    ALCint AudioSystem::stereoSources = 0;
    ALCint AudioSystem::maxAux = 0;


    void AudioSystem::setup()
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

        createProcess<&AudioSystem::update>("Update");
        bindToEvent<events::component_creation<audio_source>, &AudioSystem::onAudioSourceComponentCreate>();
        bindToEvent<events::component_destruction<audio_source>, &AudioSystem::onAudioSourceComponentDestroy>();
        bindToEvent<events::component_creation<audio_listener>, &AudioSystem::onAudioListenerComponentCreate>();
        bindToEvent<events::component_destruction<audio_listener>, &AudioSystem::onAudioListenerComponentDestroy>();
        bindToEvent<events::exit, &AudioSystem::onEngineExit>();

        // Release context on this thread
        alcMakeContextCurrent(nullptr);
    }

    void AudioSystem::onEngineExit(events::exit& event)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        static ecs::filter<audio_source> sourceQuery;
        for (auto entity : sourceQuery)
        {
            entity.remove_component<audio_source>();
        }

        alDevice = alcGetContextsDevice(alcContext);
        alcMakeContextCurrent(nullptr);
        AudioSegmentCache::unload();
        alcDestroyContext(alcContext);
        alcCloseDevice(alDevice);
    }

    void AudioSystem::queryInformation()
    {
        // This function is called from setup()
        // In setup everything is still single threaded and therefore no lock is required

        cstring vendor = alGetString(AL_VENDOR);
        cstring version = alGetString(AL_VERSION);
        cstring renderer = alGetString(AL_RENDERER);
        std::string openALExtensions = alGetString(AL_EXTENSIONS);
        std::regex match(" ");
        openALExtensions = std::regex_replace(openALExtensions, match, "\n\t\t");

        std::string ALCExtensions = alcGetString(alDevice, ALC_EXTENSIONS);
        ALCExtensions = std::regex_replace(ALCExtensions, match, "\n\t\t");

        ALCint srate;
        alcGetIntegerv(alDevice, ALC_FREQUENCY, 1, &srate);

        ALCint size;
        alcGetIntegerv(alDevice, ALC_ATTRIBUTES_SIZE, 1, &size);
        std::vector<ALCint> attrs(size);
        alcGetIntegerv(alDevice, ALC_ALL_ATTRIBUTES, size, &attrs[0]);

        for (size_t i = 0; i < attrs.size(); i++)
        {
            switch (attrs[i])
            {
            case ALC_FREQUENCY:
                frequency = attrs[++i];
                break;
            case ALC_REFRESH:
                refresh = attrs[++i];
                break;
            case ALC_SYNC:
                sync = attrs[++i];
                break;
            case ALC_MONO_SOURCES:
                monoSources = attrs[++i];
                break;
            case ALC_STEREO_SOURCES:
                stereoSources = attrs[++i];
                break;
            case ALC_MAX_AUXILIARY_SENDS:
                maxAux = attrs[++i];
                break;
            }
        }

        log::info(
            "Initialized OpenAL\n"\
            "\tCONTEXT INFO\n"\
            "\t----------------------------------\n"\
            "\tVendor:\t\t\t{}\n"\
            "\tVersion:\t\t{}\n"\
            "\tRenderer:\t\t{}\n"\
            "\tDevice samplerate:\t{}\n"\
            "\tFrequency:\t\t{}\n"\
            "\tRefresh rate:\t\t{}\n"\
            "\tSync:\t\t\t{}\n"\
            "\tMono sources:\t\t{}\n"\
            "\tStereo sources:\t\t{}\n"\
            "\tMax Auxiliary sends:\t{}\n"\
            "\tOpenAl Extensions:\n"\
            "\t\t{}\n"\
            "\n"\
            "\tALC Extensions:\n"\
            "\t\t{}\n"\
            "\t----------------------------------\n",
            vendor, version, renderer, srate, frequency, refresh, sync, monoSources, stereoSources, maxAux, openALExtensions, ALCExtensions);
    }

    void AudioSystem::update(time::span deltatime)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        static ecs::filter<audio_source> sourceQuery;
        for (auto entity : sourceQuery)
        {
            auto sourceHandle = entity.get_component<audio_source>();

            audio_source& source = sourceHandle.get();

            if (source.m_sourceId == audio_source::invalid_source_id)
                continue;

            const position& p = entity.get_component<position>();
            position& previousP = m_sourcePositions.at(sourceHandle);
            const math::vec3 vel = previousP - p;
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

            if (source.m_changes & change::doRewind)
            {
                alSourceRewind(source.m_sourceId);
            }

            if (source.m_changes & change::rollOffFactor)
            {
                alSourcef(source.m_sourceId, AL_ROLLOFF_FACTOR, source.m_rolloffFactor);
            }
            
            if (source.m_changes & change::rollOffDistance)
            {
                alSourcef(source.m_sourceId, AL_REFERENCE_DISTANCE, source.m_referenceDistance);
                alSourcef(source.m_sourceId, AL_MAX_DISTANCE, source.m_maxDistance);
            }

            if (source.m_changes & change::looping)
            {
                alSourcei(source, AL_LOOPING, static_cast<int>(source.m_looping));
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

            source.clearChanges();

            ALenum isPlaying;
            alGetSourcei(source.m_sourceId, AL_SOURCE_STATE, &isPlaying);
            if (isPlaying == AL_STOPPED) {
                source.m_playState = audio_source::stopped;
                source.m_nextPlayState = audio_source::stopped;
            }

            openal_error();
        }

        if (m_listenerEnt)
        {
            const position& p = m_listenerEnt.get_component<position>();
            const rotation& r = m_listenerEnt.get_component<rotation>();

            setListener(p, r);

            math::vec3 vel = m_listenerPosition - p;
            m_listenerPosition = p;
            alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
        }

        alcMakeContextCurrent(nullptr);
    }

#pragma region Component creation & destruction
    void AudioSystem::onAudioSourceComponentCreate(events::component_creation<audio_source>& event)
    {
        auto handle = event.entity.get_component<audio_source>();
        audio_source& a = handle.get();

        // do something with a.
        initSource(a);

        if (a.m_sourceId == audio_source::invalid_source_id)
        {
            handle.destroy();
            return;
        }

        m_sourcePositions.emplace(handle, event.entity.get_component<position>().get());

        ++sourceCount;
    }


    void AudioSystem::onAudioSourceComponentDestroy(events::component_destruction<audio_source>& event)
    {
        auto handle = event.entity.get_component<audio_source>();
        const audio_source& a = handle.get();

        if (a.m_sourceId == audio_source::invalid_source_id)
            return;

        m_sourcePositions.erase(handle);

        if (a.m_playState != audio_source::playstate::stopped)
            alSourceStop(a.m_sourceId);

        alSourcei(a.m_sourceId, AL_BUFFER, NULL);
        alDeleteSources(1, &a.m_sourceId); // Clear source
        --sourceCount;
    }

    void AudioSystem::onAudioListenerComponentCreate(events::component_creation<audio_listener>& event)
    {
        log::debug("Creating Audio Listener...");

        ++listenerCount;
        if (listenerCount > 1)
        {
            event.entity.remove_component<audio_listener>();
        }
        else
        {
            // listener count == 1
            m_listenerEnt = event.entity;
            setListener(event.entity.get_component<position>(), event.entity.get_component<rotation>());
            m_listenerPosition = event.entity.get_component<position>();
        }
    }

    void AudioSystem::onAudioListenerComponentDestroy(events::component_destruction<audio_listener>& event)
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

    void AudioSystem::initSource(audio_source& source)
    {
        static size_type i = 0;
        if (++i >= monoSources)
        {
            source.m_sourceId = audio_source::invalid_source_id;
            return;
        }

        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);

        alGenSources((ALuint)1, &source.m_sourceId);
        alSourcef(source.m_sourceId, AL_PITCH, source.m_pitch);
        alSourcef(source.m_sourceId, AL_GAIN, source.m_gain);
        alSourcei(source.m_sourceId, AL_LOOPING, source.m_looping ? AL_TRUE : AL_FALSE);

        // 3D audio stuffs
        alSourcef(source.m_sourceId, AL_ROLLOFF_FACTOR, source.m_pitch);
        alSourcef(source.m_sourceId, AL_REFERENCE_DISTANCE, source.m_referenceDistance);
        alSourcef(source.m_sourceId, AL_MAX_DISTANCE, source.m_maxDistance);

        // NOTE TO SELF:
        //      Set position and velocity to entity position and velocty
        alSource3f(source.m_sourceId, AL_POSITION, 0, 0, 0);
        alSource3f(source.m_sourceId, AL_VELOCITY, 0, 0, 0);

        if (source.m_audio_handle)
        {
            auto [segmentLock, segment] = source.m_audio_handle.get();
            async::readwrite_guard segmentGuard(segmentLock);
            alSourcei(source.m_sourceId, AL_BUFFER, segment.audioBufferId);

            if (source.m_changes & audio_source::sound_properties::playState)
            {
                using state = audio_source::playstate;
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
        }
        source.clearChanges();
        alcMakeContextCurrent(nullptr);
    }

    void AudioSystem::setDistanceModel(ALenum distanceModel)
    {
        std::lock_guard guard(contextLock);
        alcMakeContextCurrent(alcContext);
        alDistanceModel(distanceModel);
        alcMakeContextCurrent(nullptr);
    }

    void AudioSystem::setListener(position p, rotation r)
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
