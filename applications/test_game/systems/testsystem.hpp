#pragma once
#include <core/core.hpp>

using namespace args;

struct sah
{
	int value;

	sah operator+(const sah& other)
	{
		return { value + other.value };
	}

	sah operator*(const sah& other)
	{
		return { value * other.value };
	}
};


class TestSystem final : public System<TestSystem>
{
public:
	virtual void setup()
	{
		auto ent = m_ecs->createEntity();
		ent.add_component<sah>();

		createProcess<&TestSystem::update>("Update");
		createProcess<&TestSystem::differentThread>("TestChain");
		createProcess<&TestSystem::differentInterval>("TestChain", 1.f);
	}

	void update(time::time_span<fast_time> deltaTime)
	{
		static auto query = createQuery<sah>();

		static time::time_span<fast_time> buffer;
		static int frameCount;
		static time::time_span<fast_time> accumulated;

		for (auto entity : query)
		{
			auto comp = entity.get_component<sah>();
			comp.write({ frameCount });
			std::cout << comp.read().value << std::endl;
		}

		buffer += deltaTime;
		accumulated += deltaTime;
		frameCount++;

		if (buffer > 1.f)
		{
			buffer -= 1.f;
			std::cout << "Hi! " << (frameCount / accumulated) << "fps " << deltaTime.milliseconds() << "ms" << std::endl;
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

		if (accumulated > 10.f)
		{
			std::cout << "raising exit event" << std::endl;
			raiseEvent<events::exit>();
			//throw args_exception_msg("hehehe fuck you >:D");
		}
	}
};