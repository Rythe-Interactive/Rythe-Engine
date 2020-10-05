#pragma once

namespace args::audio
{
	inline void AudioSystem::setup()
	{
		m_contextLock = async::readonly_rw_spinlock();
		async::readwrite_guard guard(m_contextLock);

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

		alContext = alcCreateContext(alDevice, NULL);
		if (!alcMakeContextCurrent(alContext))
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
		m_listenerPosition = position(0,0,0);
		log::info("initialized listener!");

		alDopplerFactor(1.0f);
		alDistanceModel(AL_EXPONENT_DISTANCE);

		queryInformation();

		//m_sourcePositions = std::unordered_map<ecs::component_handle<audio_source>, position>();

		//ARGS function binding

		sourceQuery = createQuery<audio_source>();

		createProcess<&AudioSystem::update>("Audio");
		bindToEvent<events::component_creation<audio_source>, &AudioSystem::onAudioSourceComponentCreate>();
		bindToEvent<events::component_destruction<audio_source>, &AudioSystem::onAudioSourceComponentDestroy>();
		bindToEvent<events::component_creation<audio_listener>, &AudioSystem::onAudioListenerComponentCreate>();
		bindToEvent<events::component_destruction<audio_listener>, &AudioSystem::onAudioListenerComponentDestroy>();

		// Release context on this thread
		alcMakeContextCurrent(nullptr);
	}

	inline AudioSystem::~AudioSystem()
	{
	}

	inline void AudioSystem::queryInformation()
	{
		const ALchar* vendor = alGetString(AL_VENDOR);
		const ALchar* version = alGetString(AL_VERSION);
		const ALchar* renderer = alGetString(AL_RENDERER);
		const ALchar* openALExtensions = alGetString(AL_EXTENSIONS);
		const ALchar* ALCExtensions = alcGetString(alDevice, ALC_EXTENSIONS);
		ALCint srate;
		alcGetIntegerv(alDevice, ALC_FREQUENCY, 1, &srate);
		log::info("OpenAL info:\n\n\t\t\tOpenAL information\n\tVendor: {}\n\tVersion: {}\n\tRenderer: {}\n\tOpenAl Extensions: {}\n\tALC Extensions: {}\n\tDevice samplerate: {}\n",
			vendor, version, renderer, openALExtensions, ALCExtensions, srate);
	}

	inline void AudioSystem::update(time::span deltatime)
	{
		async::readonly_guard guard(m_contextLock);
		alcMakeContextCurrent(alContext);

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
			log::debug(source.m_changes);
			if (source.m_changes & 1)
			{
				// Pitch has changed
				alSourcef(source.m_sourceId, AL_PITCH, source.getPitch());
			}
			if (source.m_changes & 2)
			{
				// Gain has changed
				alSourcef(source.m_sourceId, AL_GAIN, source.getGain());
			}

			source.clearChanges();
			sourceHandle.write(source);
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
		if (!initSource(a))
		{
			handle.destroy();
#if defined(AUDIO_EXIT_ON_FAIL)
			raiseEvent<events::exit>();
#endif
			return;
		}

		{
			async::readwrite_guard guard(m_contextLock);
			alcMakeContextCurrent(alContext);
			// NOTE TO SELF:
			// REMOVE THE AUTO PLAY (TESTING PURPOSE)
			log::debug("playing sound");
			alSourcePlay(a.m_sourceId);
			alcMakeContextCurrent(nullptr);
		}

		m_sourcePositions.emplace(handle, event->entity.read_component<position>());

		handle.write(a);
		++sourceCount;
	}


	inline void AudioSystem::onAudioSourceComponentDestroy(events::component_destruction<audio_source>* event)
	{
		auto handle = event->entity.get_component_handle<audio_source>();
		audio_source a = handle.read();

		m_sourcePositions.erase(handle);

		handle.write(a);
		--sourceCount;
	}

	inline void AudioSystem::onAudioListenerComponentCreate(events::component_creation<audio_listener>* event)
	{

		log::debug("Creating Audio Listener...");

		auto handle = event->entity.get_component_handle<audio_listener>();
		audio_listener a = handle.read();
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
		handle.write(a);
	}

	inline void AudioSystem::onAudioListenerComponentDestroy(events::component_destruction<audio_listener>* event)
	{
		log::debug("Destroying Audio Listener...");

		listenerCount = math::max((int)(listenerCount-1), 0);
		if (listenerCount == 0)
		{
			log::debug("No Listeners left, resetting listener");
			m_listenerEnt = ecs::entity_handle();
			// Reset listener
			async::readwrite_guard guard(m_contextLock);
			alcMakeContextCurrent(alContext);
			alListener3f(AL_POSITION, 0, 0, 0);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			ALfloat ori[] = { 0, 0, 1.0f, 0, 1.0f, 0 };
			alListenerfv(AL_ORIENTATION, ori);
		}
	}


#pragma endregion

	inline bool AudioSystem::initSource(audio_source& source)
	{
		async::readwrite_guard guard(m_contextLock);
		alcMakeContextCurrent(alContext);

		alGenSources((ALuint)1, &source.m_sourceId);
		alSourcef(source.m_sourceId, AL_PITCH, 1);
		alSourcef(source.m_sourceId, AL_GAIN, 1);
		alSourcef(source.m_sourceId, AL_LOOPING, AL_TRUE);

		// 3D audio stuffs
		alSourcef(source.m_sourceId, AL_ROLLOFF_FACTOR, 2.0f);
		alSourcef(source.m_sourceId, AL_REFERENCE_DISTANCE, 6);
		alSourcef(source.m_sourceId, AL_MAX_DISTANCE, 15);

		// NOTE TO SELF:
		//		Set position and velocity to entity position and velocty
		alSource3f(source.m_sourceId, AL_POSITION, 0, 0, 0);
		alSource3f(source.m_sourceId, AL_VELOCITY, 0, 0, 0);

		// NOTE TO SELF:
		//		Move audio loading somewhere else
		//		This is here for testing purposes

		alGenBuffers((ALuint)1, &source.m_audioBufferId);

		auto [lock, segment] = source.m_audio_handle.get();

		{
			async::readonly_guard guard(lock);
			alBufferData(source.m_audioBufferId, AL_FORMAT_MONO16, segment.buffer, segment.samples * sizeof(int16), segment.sampleRate);
		}
		
		alSourcei(source.m_sourceId, AL_BUFFER, source.m_audioBufferId);
		alcMakeContextCurrent(nullptr);

		return true;
	}

	inline void AudioSystem::setDistanceModel(ALenum distanceModel)
	{
		alDistanceModel(distanceModel);
	}

	inline void AudioSystem::setListener(position p, rotation r)
	{
		async::readwrite_guard guard(m_contextLock);
		alcMakeContextCurrent(alContext);
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
