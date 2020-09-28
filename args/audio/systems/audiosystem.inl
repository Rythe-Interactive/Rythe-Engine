#pragma once

namespace args::audio
{
	inline void AudioSystem::update(time::span deltatime)
	{
		for (auto entity : sourceQuery)
		{
			auto sourceHandle = entity.get_component_handle<audio_source>();
			auto posHandle = entity.get_component_handle<position>();

			//audio_source a = sourceHandle.read();
		}

		for (auto entity : listenerQuery)
		{
			auto listenerHandle = entity.get_component_handle<audio_listener>();
			auto positionHandle = entity.get_component_handle<position>();
			auto rotationHandle = entity.get_component_handle<rotation>();

			position p = positionHandle.read();
			rotation r = rotationHandle.read();

			alListener3f(AL_POSITION, p.x, p.y, p.z);
			alListener3f(AL_VELOCITY, 0, 0, 0);
			math::mat3 mat3 = math::toMat3(r);
			math::vec3 forward = mat3 * math::vec3(0.f, 0.f, 1.f);
			math::vec3 up = mat3 * math::vec3(0.f, 1.f, 0.f);
			ALfloat ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
			alListenerfv(AL_ORIENTATION, ori);
		}
	}

	inline bool AudioSystem::initSource(audio_source& source)
	{
		ALuint sourceId;
		alGenSources((ALuint)1, &sourceId);
		source.m_sourceId = sourceId;
		alSourcef(sourceId, AL_PITCH, 1);
		alSourcef(sourceId, AL_GAIN, 1);
		alSourcef(sourceId, AL_LOOPING, AL_TRUE);

		// 3D audio stuffs
		alSourcef(sourceId, AL_ROLLOFF_FACTOR, 2.0f);
		alSourcef(sourceId, AL_REFERENCE_DISTANCE, 6);
		alSourcef(sourceId, AL_MAX_DISTANCE, 15);

		// NOTE TO SELF:
		//		Set position and velocity to entity position and velocty
		alSource3f(sourceId, AL_POSITION, 0, 0, 0);
		alSource3f(sourceId, AL_VELOCITY, 0, 0, 0);

		// NOTE TO SELF:
		//		Move audio loading somewhere else
		//		This is here for testing purposes

		ALuint buffer;
		alGenBuffers((ALuint)1, &buffer);
		source.m_audioBufferId = buffer;

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

		alBufferData(buffer, AL_FORMAT_MONO16, source.m_audioInfo.buffer, source.m_audioInfo.samples * sizeof(mp3d_sample_t), source.m_audioInfo.hz);
		alSourcei(sourceId, AL_BUFFER, buffer);
		return true;
	}
}
