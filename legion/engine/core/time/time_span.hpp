#pragma once
#include <chrono>

#include <core/platform/platform.hpp>
#include <core/math/math.hpp>
#include <core/types/primitives.hpp>

namespace legion::core::time
{
    template<typename precision = fast_time>
    struct time_span
    {
        using time_type = precision;
        using duration_type = std::chrono::duration<time_type>;

        duration_type duration = duration_type::zero();

        constexpr time_span() noexcept = default;

        constexpr time_span(time_type other) noexcept : duration(other) {}

        template<typename other_time, typename ratio>
        constexpr explicit time_span(const std::chrono::duration<other_time, ratio>& other) noexcept
            : duration(std::chrono::duration_cast<duration_type>(other)) {}
        template<typename other_time CNDOXY(std::enable_if_t<!std::is_same_v<other_time, time_type>, int> = 0)>
        constexpr explicit time_span(const time_span<other_time>& other) noexcept : duration(other.duration) {}

        template<typename other_time, typename ratio>
        constexpr explicit time_span(std::chrono::duration<other_time, ratio>&& other) noexcept
            : duration(std::chrono::duration_cast<duration_type>(other)) {}
        template<typename other_time CNDOXY(std::enable_if_t<!std::is_same_v<other_time, time_type>, int> = 0)>
        constexpr explicit time_span(time_span<other_time>&& other) noexcept : duration(std::move(other.duration)) {}

        template<typename other_time CNDOXY(std::enable_if_t<!std::is_same_v<other_time, time_type>, int> = 0)>
        constexpr explicit time_span(other_time other) noexcept : duration(other) {}

        template<typename other_time>
        constexpr time_span<other_time> cast() const noexcept { return { *this }; }

        template<typename T>
        L_NODISCARD constexpr T hours() const noexcept(std::is_arithmetic_v<time_type>&& std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<3600>>>(duration).count(); }
        template<typename T>
        L_NODISCARD constexpr T minutes() const noexcept(std::is_arithmetic_v<time_type>&& std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T, std::ratio<60>>>(duration).count(); }
        template<typename T>
        L_NODISCARD constexpr T seconds() const noexcept(std::is_arithmetic_v<time_type>&& std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T>>(duration).count(); }
        template<typename T>
        L_NODISCARD constexpr T milliseconds() const noexcept(std::is_arithmetic_v<time_type>&& std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T, std::milli>>(duration).count(); }
        template<typename T>
        L_NODISCARD constexpr T microseconds() const noexcept(std::is_arithmetic_v<time_type>&& std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T, std::micro>>(duration).count(); }
        template<typename T>
        L_NODISCARD constexpr T nanoseconds() const noexcept(std::is_arithmetic_v<time_type> && std::is_arithmetic_v<T>) { return std::chrono::duration_cast<std::chrono::duration<T, std::nano>>(duration).count(); }

        L_NODISCARD constexpr time_type hours() const noexcept(std::is_arithmetic_v<time_type>) { return std::chrono::duration_cast<std::chrono::duration<time_type, std::ratio<3600>>>(duration).count(); }
        L_NODISCARD constexpr time_type minutes() const noexcept(std::is_arithmetic_v<time_type>) { return std::chrono::duration_cast<std::chrono::duration<time_type, std::ratio<60>>>(duration).count(); }
        L_NODISCARD constexpr time_type seconds() const noexcept(std::is_arithmetic_v<time_type>) { return duration.count(); }
        L_NODISCARD constexpr time_type milliseconds() const noexcept(std::is_arithmetic_v<time_type>) { return std::chrono::duration_cast<std::chrono::duration<time_type, std::milli>>(duration).count(); }
        L_NODISCARD constexpr time_type microseconds() const noexcept(std::is_arithmetic_v<time_type>) { return std::chrono::duration_cast<std::chrono::duration<time_type, std::micro>>(duration).count(); }
        L_NODISCARD constexpr time_type nanoseconds() const noexcept(std::is_arithmetic_v<time_type>) { return std::chrono::duration_cast<std::chrono::duration<time_type, std::nano>>(duration).count(); }

        L_NODISCARD constexpr operator duration_type&() noexcept
        {
            return duration;
        }

        L_NODISCARD constexpr operator const duration_type&() const noexcept
        {
            return duration;
        }

        L_NODISCARD constexpr operator time_type() const noexcept
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

        template<typename OtherPrecision>
        constexpr time_span& operator+=(const time_span<OtherPrecision>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration += std::chrono::duration_cast<duration_type>(rhs.duration);
            return *this;
        }

        template<typename OtherPrecision>
        constexpr time_span& operator-=(const time_span<OtherPrecision>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration -= std::chrono::duration_cast<duration_type>(rhs.duration);
            return *this;
        }

        template<typename OtherPrecision>
        constexpr time_span& operator*=(const time_span<OtherPrecision>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration *= std::chrono::duration_cast<duration_type>(rhs.duration);
            return *this;
        }

        template<typename OtherPrecision>
        constexpr time_span& operator/=(const time_span<OtherPrecision>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration /= std::chrono::duration_cast<duration_type>(rhs.duration);
            return *this;
        }

        template<typename OtherPrecision>
        constexpr time_span& operator%=(const time_span<OtherPrecision>& rhs) noexcept(std::is_arithmetic_v<time_type>) /* strengthened */
        {
            duration %= std::chrono::duration_cast<duration_type>(rhs.duration);
            return *this;
        }

        L_NODISCARD static constexpr time_span zero() noexcept
        {
            // get zero value
            return time_span(0);
        }

        L_NODISCARD static constexpr time_span min() noexcept
        {
            // get minimum value
            return time_span((std::chrono::duration_values<time_type>::min)());
        }

        L_NODISCARD static constexpr time_span max() noexcept
        {
            // get maximum value
            return time_span((std::chrono::duration_values<time_type>::max)());
        }
    };

    using span = time_span<>;
}

template <typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>
operator+(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    using common_span = legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>;
    return common_span(lhs.duration + rhs.duration);
}

template <typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>
operator-(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    using common_span = legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>;
    return common_span(lhs.duration - rhs.duration);
}

template <typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>
operator*(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    using common_span = legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>;
    return common_span(lhs.duration * rhs.duration);
}

template <typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>
operator/(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    using common_span = legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>;
    return common_span(lhs.duration / rhs.duration);
}

template <typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>
operator%(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    using common_span = legion::core::time::time_span<std::common_type_t<PrecisionType1, PrecisionType2>>;
    return common_span(lhs.duration % rhs.duration);
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD bool
operator==(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return legion::core::math::close_enough(lhs.duration.count(), rhs.duration.count());
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD bool
operator!=(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return !(lhs == rhs);
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr bool
operator<(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
    noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return lhs.duration < rhs.duration;
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr bool
operator<=(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return lhs.duration <= rhs.duration;
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr bool
operator>(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return lhs.duration > rhs.duration;
}

template<typename PrecisionType1, typename PrecisionType2>
L_NODISCARD constexpr bool
operator>=(const legion::core::time::time_span<PrecisionType1>& lhs, const legion::core::time::time_span<PrecisionType2>& rhs)
noexcept(std::is_arithmetic_v<PrecisionType1>&& std::is_arithmetic_v<PrecisionType2>) /* strengthened */
{
    return lhs.duration >= rhs.duration;
}
