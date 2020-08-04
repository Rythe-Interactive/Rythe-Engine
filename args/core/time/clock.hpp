#pragma once
#include <chrono>
#include <core/time/time_span.hpp>
#include <core/types/primitives.hpp>

namespace args::core::time
{
	template<typename time_type = time64>
	class clock
	{
	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	public:
		void Start() {
			start = std::chrono::high_resolution_clock::now();
		}

		time_span<time_type> GetElapsedTime()
		{
			return time_span<time_type>(std::chrono::high_resolution_clock::now() - start);
		}

		time_span<time_type> End()
		{
			return time_span<time_type>(std::chrono::high_resolution_clock::now() - start);
		}

		time_span<time_type> Restart()
		{
			auto startTime = std::chrono::high_resolution_clock::now();
			time_span<time_type> time(startTime - start);
			start = startTime;
			return time;
		}
	};
}