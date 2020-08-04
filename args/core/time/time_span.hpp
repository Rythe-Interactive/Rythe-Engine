#pragma once
#include <chrono>
#include <core/types/primitives.hpp>

namespace args::core::time
{
	template<typename time_type>
	class time_span
	{
	private:
		std::chrono::duration<time_type> m_duration;

	public:
		time_span(std::chrono::duration<time_type> duration) : m_duration(duration) {}

		template<typename T>
		T hours() { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<3600>>>(m_duration).count(); }
		template<typename T>
		T minutes() { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<60>>>(m_duration).count(); }
		template<typename T>
		T seconds() { return std::chrono::duration_cast<std::chrono::duration<T>>(m_duration).count(); }
		template<typename T>
		T milliseconds() { return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(m_duration).count(); }

		fast_time hours() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::ratio<3600>>>(m_duration).count(); }
		fast_time minutes() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::ratio<60>>>(m_duration).count(); }
		fast_time seconds() { return std::chrono::duration_cast<std::chrono::duration<fast_time>>(m_duration).count(); }
		fast_time milliseconds() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::milli>>(m_duration).count(); }
	};
}