#pragma once
#include <chrono>
#include <core/types/primitives.hpp>

namespace legion::core::time
{
	template<typename time_type = fast_time>
	struct time_span
	{
		std::chrono::duration<time_type> duration = std::chrono::duration<time_type>::zero();

        time_span() = default;

		template<typename other_time>
		constexpr time_span(const std::chrono::duration<other_time>& other) noexcept : duration(other) {}
		template<typename other_time>
        constexpr time_span(const time_span<other_time>& other) noexcept : duration(other.duration) {}

		template<typename other_time>
        constexpr time_span(std::chrono::duration<other_time>&& other) noexcept : duration(other) {}
		template<typename other_time>
        constexpr time_span(time_span<other_time>&& other) noexcept : duration(std::move(other.duration)) {}

		template<typename other_time>
        constexpr time_span(other_time other) noexcept : duration(other) {}

        explicit time_span(time_type duration) noexcept : duration(duration) {}

		template<typename T>
		T hours() { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<3600>>>(duration).count(); }
		template<typename T>
		T minutes() { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<60>>>(duration).count(); }
		template<typename T>
		T seconds() { return std::chrono::duration_cast<std::chrono::duration<T>>(duration).count(); }
		template<typename T>
		T milliseconds() { return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(duration).count(); }
		template<typename T>
		T microseconds() { return std::chrono::duration_cast<std::chrono::duration<T, std::micro>>(duration).count(); }
		template<typename T>
		T nanoseconds() { return std::chrono::duration_cast<std::chrono::duration<T, std::nano>>(duration).count(); }

		fast_time hours() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::ratio<3600>>>(duration).count(); }
		fast_time minutes() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::ratio<60>>>(duration).count(); }
		fast_time seconds() { return std::chrono::duration_cast<std::chrono::duration<fast_time>>(duration).count(); }
		fast_time milliseconds() { return std::chrono::duration_cast<std::chrono::duration<fast_time, std::milli>>(duration).count(); }	
		fast_micro_seconds microseconds() { return std::chrono::duration_cast<std::chrono::duration<fast_micro_seconds, std::micro>>(duration).count(); }
		fast_nano_seconds nanoseconds() { return std::chrono::duration_cast<std::chrono::duration<fast_nano_seconds, std::nano>>(duration).count(); }

        operator std::chrono::duration<time_type>&()
        {
            return duration;
        }

        operator std::chrono::duration<time_type>() const
        {
            return duration;
        }

        operator time_type() const
        {
            return duration.count();
        }

        constexpr time_span& operator++() noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            ++duration;
            return *this;
        }

        constexpr time_span operator++(int) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            return time_span(duration++);
        }

        constexpr time_span& operator--() noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            --duration;
            return *this;
        }

        constexpr time_span operator--(int) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            return time_span(duration--);
        }

        constexpr time_span& operator+=(const time_span& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration += rhs.duration;
            return *this;
        }

        constexpr time_span& operator-=(const time_span& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration -= rhs.duration;
            return *this;
        }

        constexpr time_span& operator*=(const time_type& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration *= rhs;
            return *this;
        }

        constexpr time_span& operator/=(const time_type& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration /= rhs;
            return *this;
        }

        constexpr time_span& operator%=(const time_type& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration %= rhs;
            return *this;
        }

        constexpr time_span& operator%=(const std::chrono::duration<time_type>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */ 
        {
            duration %= rhs.count();
            return *this;
        }

        L_NODISCARD static constexpr time_span zero() noexcept
        {
            // get zero value
            return time_span(0);
        }

        L_NODISCARD static constexpr time_span(min)() noexcept
        {
            // get minimum value
            return time_span((std::chrono::duration_values<time_type>::min)());
        }

        L_NODISCARD static constexpr time_span(max)() noexcept
        {
            // get maximum value
            return time_span((std::chrono::duration_values<time_type>::max)());
        }
	};

    using span = time_span<>;
}
