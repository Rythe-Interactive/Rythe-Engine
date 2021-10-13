#pragma once

#include <core/math/precision.hpp>
#include <core/platform/platform.hpp>
#include <core/math/constants.hpp>
#include <cmath>

namespace legion::core::math
{
#pragma region primitive
    template<class T>
    constexpr T rad2deg_c()
    {
        return T(180) / pi<T>();
    }

    template <class T>
    constexpr T rad2deg(T v)
    {
        return rad2deg_c<T>() * v;
    }


    template <class T>
    constexpr T deg2rad_c()
    {
        return pi<T>() / T(180);
    }

    template <class T>
    constexpr T deg2rad(T v)
    {
        return deg2rad_c<T>() * v;
    }
#pragma endregion

    /**@class angle
     * @tparam p The data_precision of the angle class (typically bit32).
     * @brief Represents an angle does the rad/deg conversion internally so that
     *        the user does not have to worry about it.
     */
    template <data_precision p = data_precision::bit32>
    class angle
    {
    public:
        using storage_type = precision_chooser_t<p>;

        angle(const angle& other) = default;
        angle(angle&& other) noexcept = default;
        angle& operator=(const angle& other) = default;
        angle& operator=(angle&& other) noexcept = default;
        ~angle() = default;

        /** @brief Constructs an angle from a degree value.
         */
        static constexpr angle deg(storage_type value);

        /** @brief Constructs an angle from a radians value.
         */
        static constexpr angle rad(storage_type value);

        /** @brief Converts an angle to degrees.
         */
        L_NODISCARD constexpr storage_type as_degrees() const;
        
        /**@brief Converts an angle to radians
         */
        L_NODISCARD constexpr storage_type as_radians() const;

        /** @brief Applies the trigonometric sinus function on the angle
         *         and returns it's value.
         *  @note  The sinus-value of an angle is the ratio between the
         *         opposite and the hypotenuse
         */
        storage_type sin();

        /** @brief Applies the trigonometric co-sinus function on the angle
         *         and returns it's value.
         *  @note  The co-sinus-value of an angle is the ratio between the
         *         adjacent and the hypotenuse
         */
        storage_type cos();

        /** @brief Applies the trigonometric tangent function on the angle
         *         and returns it's value.
         *  @note  The tangent-value of an angle is the ratio between the
         *         adjacent and the opposite
         */
        storage_type tan();

        /** @brief Applies the arcsin function to a sin value and constructs
         *         and angle from it.
         *  @note  The arcsin is the counterpart to the sinus function and reverses
         *         it's effects.
         */
        static angle arcsin(storage_type v);
        
        /** @brief Applies the arccos function to a cos value and constructs
         *         and angle from it.
         *  @note  The arccos is the counterpart to the co-sinus function and reverses
         *         it's effects.
         */
        static angle arccos(storage_type v);

        /** @brief Applies the arctan function to a tan value and constructs
         *         and angle from it.
         *  @note  The arctan is the counterpart to the tangent function and reverses
         *         it's effects.
         */
        static angle arctan(storage_type v);

        /** @brief Applies the arctan2 function to a pair of cartesian coordinates,
         *         and returns the polar-angle of those coordinates in respect to
         *         the origin (0,0)
         *  @note  The arctan is the counterpart to the tangent function and reverses
         *         it's effects.
         */
        static angle arctan2(storage_type y, storage_type x);

    private:
        angle(storage_type v) : m_angle_value(v) {}

        //the default is radians!
        storage_type m_angle_value;
    };

    template <data_precision p>
    typename angle<p>::storage_type angle<p>::sin()
    {
        return std::sin(m_angle_value);
    }

    template <data_precision p>
    typename angle<p>::storage_type angle<p>::cos()
    {
        return std::cos(m_angle_value);
    }

    template <data_precision p>
    typename angle<p>::storage_type angle<p>::tan()
    {
        return std::tan(m_angle_value);
    }

    template <data_precision p>
    angle<p> angle<p>::arcsin(storage_type v)
    {
        return angle<p>::rad(std::asin(v));
    }

    template <data_precision p>
    angle<p> angle<p>::arccos(storage_type v)
    {
        return angle<p>::rad(std::acos(v));
    }

    template <data_precision p>
    angle<p> angle<p>::arctan(storage_type v)
    {
        return angle<p>::rad(std::atan(v));
    }

    template <data_precision p>
    angle<p> angle<p>::arctan2(storage_type y, storage_type x)
    {
        return angle<p>::rad(std::atan2(y, x));
    }


    template <data_precision p>
    constexpr angle<p> angle<p>::deg(storage_type value)
    {
        return angle<p>(deg2rad(value));
    }

    template <data_precision p>
    constexpr angle<p> angle<p>::rad(storage_type value)
    {
        return angle<p>(value);
    }

    template <data_precision p>
    constexpr typename angle<p>::storage_type angle<p>::as_degrees() const
    {
        return rad2deg(m_angle_value);
    }

    template <data_precision p>
    constexpr typename angle<p>::storage_type angle<p>::as_radians() const
    {
        return m_angle_value;
    }

    using anglef = angle<>;
    using angled = angle<data_precision::bit64>;
    using angleld = angle<data_precision::lots>;
    
}
