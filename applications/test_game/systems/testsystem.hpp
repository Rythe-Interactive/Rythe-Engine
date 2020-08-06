#pragma once
#include <core/core.hpp>

using namespace args;

class TestSystem final : public System<TestSystem>
{
public:
	virtual void setup()
	{
		createProcess<&TestSystem::update>("Update");
		createProcess<&TestSystem::differentThread>("TestChain");
		createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
	}

	void update(time::time_span<fast_time> deltaTime)
	{
		static time::time_span<fast_time> buffer;
		static int frameCount;
		static time::time_span<fast_time> accumulated;

		buffer += deltaTime;
		accumulated += deltaTime;
		frameCount++;

		if (buffer > 1.f)
		{
			buffer -= 1.f;
			std::cout << "Hi! " << (frameCount/accumulated)  << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
		}
	}

	void differentInterval(time::time_span<fast_time> deltaTime)
	{
		static time::time_span<fast_time> buffer;
		static int frameCount;
		static time::time_span<fast_time> accumulated;

		buffer += deltaTime;
		accumulated += deltaTime;
		frameCount++;

		if (buffer > 1.f)
		{
			buffer -= 1.f;
			std::cout << "This is a fixed interval!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
		}
	}

	void differentThread(time::time_span<fast_time> deltaTime)
	{
		static time::time_span<fast_time> buffer;
		static int frameCount;
		static time::time_span<fast_time> accumulated;

		buffer += deltaTime;
		accumulated += deltaTime;
		frameCount++;

		if (buffer > 1.f)
		{
			buffer -= 1.f;
			std::cout << "This is a different thread!! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
		}
	}
};