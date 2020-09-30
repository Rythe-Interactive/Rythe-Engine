#pragma once
#define AL_ALEXT_PROTOTYPES
#include <al/alext.h>

namespace args::audio
{
	inline void AudioSystem::setup()
	{
		m_lock = new async::readonly_rw_spinlock();
		async::readwrite_guard guard(*m_lock);

		data::alDevice = alcOpenDevice(NULL);
		if (!data::alDevice)
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

		data::alContext = alcCreateContext(data::alDevice, NULL);
		if (!alcMakeContextCurrent(data::alContext))
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
		log::info("initialized listener!");

		alDopplerFactor(1.0f);
		alDistanceModel(AL_EXPONENT_DISTANCE);

		queryInformation();

		//ARGS function binding

		sourceQuery = createQuery<audio_source, position>();
		listenerQuery = createQuery<audio_listener, position, rotation>();

		createProcess<&AudioSystem::update>("Audio");
		bindToEvent<events::component_creation<audio_source>, &AudioSystem::onComponentCreate>();
		bindToEvent<events::component_destruction<audio_source>, &AudioSystem::onComponentDestroy>();

		// Release context on this thread
		alcMakeContextCurrent(nullptr);
	}

	inline void AudioSystem::queryInformation()
	{
		const ALchar* vendor = alGetString(AL_VENDOR);
		const ALchar* version = alGetString(AL_VERSION);
		const ALchar* renderer = alGetString(AL_RENDERER);
		const ALchar* openALExtensions = alGetString(AL_EXTENSIONS);
		const ALchar* ALCExtensions = alcGetString(data::alDevice, ALC_EXTENSIONS);
		ALCint srate;
		alcGetIntegerv(data::alDevice, ALC_FREQUENCY, 1, &srate);
		log::info("OpenAL info:\n\n\t\t\tOpenAL information\n\tVendor: {}\n\tVersion: {}\n\tRenderer: {}\n\tOpenAl Extensions: {}\n\tALC Extensions: {}\n\tDevice samplerate: {}\n",
			vendor, version, renderer, openALExtensions, ALCExtensions, srate);
	}

	inline void AudioSystem::update(time::span deltatime)
	{
		async::readwrite_guard guard(*m_lock);
		alcMakeContextCurrent(data::alContext);

		for (auto entity : sourceQuery)
		{
			auto sourceHandle = entity.get_component_handle<audio_source>();
			auto posHandle = entity.get_component_handle<position>();

			//audio_source a = sourceHandle.read();
		}

		if (listenerQuery.size() != 0)
		{
			auto entity = listenerQuery[0];
			auto listenerHandle = entity.get_component_handle<audio_listener>();
			auto positionHandle = entity.get_component_handle<position>();
			auto rotationHandle = entity.get_component_handle<rotation>();

			position p = positionHandle.read();
			rotation r = rotationHandle.read();

			alListener3f(AL_POSITION, p.x, p.y, p.z);
			math::vec3 vel = m_lisPos - p;
			m_lisPos = p;
			alListener3f(AL_VELOCITY, vel.x*100, vel.y*100, vel.z*100);
			math::mat3 mat3 = math::toMat3(r);
			math::vec3 forward = mat3 * math::vec3(0.f, 0.f, -1.f);
			math::vec3 up = mat3 * math::vec3(0.f, 1.f, 0.f);
			ALfloat ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
			alListenerfv(AL_ORIENTATION, ori);
		}
		

		alcMakeContextCurrent(nullptr);
	}

	inline void AudioSystem::onComponentCreate(events::component_creation<audio_source>* event)
	{
		async::readwrite_guard guard(*m_lock);
		alcMakeContextCurrent(data::alContext);

		auto handle = event->entity.get_component_handle<audio_source>();
		audio_source a = handle.read();
		// do something with a.
		if (!initSource(a))
		{
			handle.destroy();

			alcMakeContextCurrent(nullptr);

#if defined(AUDIO_EXIT_ON_FAIL)
			raiseEvent<events::exit>();
#endif
			return;
		}
		log::debug("playing sound");
		alSourcePlay(a.m_sourceId);

		handle.write(a);
		++data::sourceCount;

		alcMakeContextCurrent(nullptr);
	}


	inline void AudioSystem::onComponentDestroy(events::component_destruction<audio_source>* event)
	{
		async::readwrite_guard guard(*m_lock);
		alcMakeContextCurrent(data::alContext);

		auto handle = event->entity.get_component_handle<audio_source>();
		audio_source a = handle.read();
		handle.write(a);
		--data::sourceCount;

		alcMakeContextCurrent(nullptr);
	}

	inline bool AudioSystem::initSource(audio_source& source)
	{
		// No need for setting a lock and makeContextCurrent since this function
		// is called from another function (AudioSystem::onComponentCreate).

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

		fs::view view("assets://audio/365921__inspectorj__waterfall-small-b[mono].mp3");
		auto result = view.get();
		if (result != common::valid)
		{
			log::error("{}", result.get_error().what());
			return false;
		}

		fs::basic_resource fileContents = result;
		mp3dec_map_info_t map_info;
		map_info.buffer = fileContents.data();
		map_info.size = fileContents.size();

		if (mp3dec_load_mapinfo(&source.m_mp3dec, &map_info, &source.m_audioInfo, NULL, NULL))
		{
			log::error("Failed to load audio file: {}", view.get_path());
			return false;
		}

		alBufferData(source.m_audioBufferId, AL_FORMAT_MONO16, source.m_audioInfo.buffer, source.m_audioInfo.samples * sizeof(mp3d_sample_t), source.m_audioInfo.hz);
		alSourcei(source.m_sourceId, AL_BUFFER, source.m_audioBufferId);

		log::info("audioFile: {}\nBuffer: \t{}\nChannels: \t{}\nHz: \t\t{}\nLayer \t\t{}\nSamples: \t{}\navg kbps: \t{}\n-------------------------------------\n",
			view.get_path(),
			(void*)source.m_audioInfo.buffer,
			source.m_audioInfo.channels,
			source.m_audioInfo.hz,
			source.m_audioInfo.layer,
			source.m_audioInfo.samples,
			source.m_audioInfo.avg_bitrate_kbps);

		return true;
	}

	inline void AudioSystem::setDistanceModel(ALenum distanceModel)
	{
		alDistanceModel(distanceModel);
	}
}
