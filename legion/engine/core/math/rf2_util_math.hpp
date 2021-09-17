#pragma once
#include <cmath>
#include <type_traits>
#include <algorithm>
#include <limits>

#include "isiTypes.hpp"
#include "AdvAssert.hpp"

namespace UtilMath
{
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																											//
	//											FORWARD DECLARATION												//
	//																											//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Forward declarations

	template<typename _Scalar, size_t _Size>
	struct Vector;

	template<typename _Scalar, size_t _Size>
	Vector<_Scalar, _Size> sqrt(const Vector<_Scalar, _Size>& v);

	template<typename _Scalar, size_t _Size>
	_Scalar dot(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b);

	template<typename _Scalar, size_t _Size>
	Vector<_Scalar, _Size> cross(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b);

#pragma endregion

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																											//
	//												UTILITIES													//
	//																											//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Utilities
	// Implementation of Carmack's fast approximated inverse square root
	template<typename _Scalar>
	static _Scalar fast_inv_sqrt(_Scalar x)
	{
		static_assert(std::is_floating_point<_Scalar>::value, "Scalar type is not an IEEE 754 floating point type.");
		static_assert(std::numeric_limits<_Scalar>::is_iec559, "Scalar type is not an IEEE 754 floating point type.");

		// union hack to prevent so many reinterpret_cast's
		union {
			float f;
			isi_u32 i;
		} data;

		data.f = static_cast<float>(x);
		// 0x5f3759df is the result of the equation that allows us to remove the logarithm using an approximation of log2(1+(Mantissa/2^23))
		data.i = 0x5f3759df - (data.i >> 1); // Compute -0.5log(x), after removing the logarithm it'll be x^-0.5
		data.f = data.f * (1.5f - ((static_cast<float>(x) * 0.5f) * data.f * data.f)); // Iteration of Newtons method
		return static_cast<_Scalar>(data.f);
	}

	template<typename _Scalar>
	forceinline static _Scalar epsilon()    { return std::numeric_limits<_Scalar>::epsilon(); }

	template <typename _Scalar>
	forceinline static _Scalar euler()      { return static_cast<_Scalar>(2.7182818284590452353602874713527L); }

	template <typename _Scalar>
	forceinline static _Scalar inv_euler()  { return static_cast<_Scalar>(1) / euler<_Scalar>(); }

	template <typename _Scalar>
	forceinline static _Scalar log2e()      { return static_cast<_Scalar>(1.6931471805599453094172321214582L); }

	template <typename _Scalar>
	forceinline static _Scalar log10e()     { return static_cast<_Scalar>(3.3025850929940456840179914546844L); }

	template <typename _Scalar>
	forceinline static _Scalar pi()         { return static_cast<_Scalar>(3.1415926535897932384626433832795L); }

	template <typename _Scalar>
	forceinline static _Scalar two_pi()     { return static_cast<_Scalar>(6.2831853071795864769252867665590L); }

	template <typename _Scalar>
	forceinline static _Scalar half_pi()    { return pi<_Scalar>() * static_cast<_Scalar>(0.5); }

	template <typename _Scalar>
	forceinline static _Scalar quarter_pi() { return pi<_Scalar>() * static_cast<_Scalar>(0.25); }

	template <typename _Scalar>
	forceinline static _Scalar tau()        { return pi<_Scalar>() * static_cast<_Scalar>(2); }

	template <typename _Scalar>
	forceinline static _Scalar pau()        { return pi<_Scalar>() * static_cast<_Scalar>(1.5); }

	template <typename _Scalar>
	forceinline static _Scalar inv_pi()     { return static_cast<_Scalar>(1) / pi<_Scalar>(); }

	template <typename _Scalar>
	forceinline static _Scalar sqrt2()      { return static_cast<_Scalar>(1.4142135623730950488016887242097L); }

	template <typename _Scalar>
	forceinline static _Scalar inv_sqrt2()  { return static_cast<_Scalar>(1) / sqrt2<_Scalar>(); }

	template <typename _Scalar>
	forceinline static _Scalar sqrt3()      { return static_cast<_Scalar>(1.7320508075688772935274463415059L); }

	template <typename _Scalar>
	forceinline static _Scalar inv_sqrt3()  { return static_cast<_Scalar>(1) / sqrt3<_Scalar>(); }

	template <typename _Scalar>
	forceinline static _Scalar egamma()     { return static_cast<_Scalar>(0.5772156649015328606065120900824L); }

	template <typename _Scalar>
	forceinline static _Scalar phi()        { return static_cast<_Scalar>(1.6180339887498948482045868343656L); }

	template<typename _Scalar>
	forceinline static _Scalar rad2deg()    { return static_cast<_Scalar>(180) / pi<_Scalar>(); }

	template<typename _Scalar>
	forceinline static _Scalar deg2rad()    { return pi<_Scalar>() / static_cast<_Scalar>(180); }

#pragma endregion

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																											//
	//												VECTOR														//
	//																											//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////// DECLARATION ////////////////////////////////////////////////////
#pragma region Vector declerations

	template<typename _Scalar, size_t _Size>
	struct Vector
	{
		static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

		typedef _Scalar scalar;
		typedef Vector<_Scalar, _Size> type;

		static const type one;
		static const type zero;

		scalar data[_Size];

		Vector()                                 { for (size_t i = 0; i < _Size; i++) { data[i] = static_cast<scalar>(0); } }
		Vector(Vector&& other)                   { for (size_t i = 0; i < _Size; i++) { data[i] = other.data[i]; } }
		Vector(const Vector& other)              { for (size_t i = 0; i < _Size; i++) { data[i] = other.data[i]; } }

		explicit Vector(scalar s)                { for (size_t i = 0; i < _Size; i++) { data[i] = s; } }

		Vector operator=(Vector&& other)         { for (size_t i = 0; i < _Size; i++) { data[i] = other.data[i]; } }
		Vector operator=(const Vector& other)    { for (size_t i = 0; i < _Size; i++) { data[i] = other.data[i]; } }

		scalar& operator[](size_t i)             { ASSERT((i >= 0) && (i < _Size)); return data[i]; }
        const scalar& operator[](size_t i) const { ASSERT((i >= 0) && (i < _Size)); return data[i]; }

		template<typename _Scal>
		explicit Vector(Vector<_Scal, _Size>&& other)      { for (size_t i = 0; i < _Size; i++) { data[i] = static_cast<scalar>(other.data[i]); } }
		template<typename _Scal>
		explicit Vector(const Vector<_Scal, _Size>& other) { for (size_t i = 0; i < _Size; i++) { data[i] = static_cast<scalar>(other.data[i]); } }

		forceinline size_t size()    const { return _Size; }
		forceinline scalar length()  const { return sqrt(dot(*this, *this)); }
		forceinline scalar length2() const { return dot(*this, *this); }
	};

	template<typename _Scalar, size_t _Size>
	const Vector<_Scalar, _Size> Vector<_Scalar, _Size>::one  = Vector<_Scalar, _Size>(static_cast<_Scalar>(1));
	template<typename _Scalar, size_t _Size>
	const Vector<_Scalar, _Size> Vector<_Scalar, _Size>::zero = Vector<_Scalar, _Size>(static_cast<_Scalar>(0));

	template<typename _Scalar>
	struct Vector<_Scalar, 1>
	{
		static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

		typedef _Scalar scalar;
		typedef Vector<_Scalar, 1> type;

		static const type one;
		static const type zero;

		union
		{
			scalar x;
			scalar data[1];
		};

		Vector() : x(static_cast<scalar>(0))     {}
		Vector(Vector&& other) : x(other.x)      {}
		Vector(const Vector& other) : x(other.x) {}

		Vector operator=(Vector&& other)         { x = other.x; return *this; }
		Vector operator=(const Vector& other)    { x = other.x; return *this; }

		scalar& operator[](size_t i)             { ASSERT((i == 0)); return data[i]; }
        const scalar& operator[](size_t i) const { ASSERT((i == 0)); return data[i]; }

		template<typename _Scal>
		explicit Vector(Vector<_Scal, 1>&& other)      : x(static_cast<scalar>(other.x)) {}
		template<typename _Scal>
		explicit Vector(const Vector<_Scal, 1>& other) : x(static_cast<scalar>(other.x)) {}

		Vector(scalar s) : x(s)    {}
		Vector operator=(scalar s) { x = s; return *this; }
		operator scalar()          { return x; }

		forceinline size_t size()    const { return 1; }
		forceinline scalar length()  const { return x; }
		forceinline scalar length2() const { return x * x; }
	};

	template<typename _Scalar>
	const Vector<_Scalar, 1> Vector<_Scalar, 1>::one  = Vector<_Scalar, 1>(static_cast<_Scalar>(1));
	template<typename _Scalar>
	const Vector<_Scalar, 1> Vector<_Scalar, 1>::zero = Vector<_Scalar, 1>(static_cast<_Scalar>(0));

	template<typename _Scalar>
	struct Vector<_Scalar, 2>
	{
		static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

		typedef _Scalar scalar;
		typedef Vector<_Scalar, 2> type;

		static const type up;
		static const type down;
		static const type right;
		static const type left;
		static const type one;
		static const type zero;

		union
		{
			struct
			{
				scalar x, y;
			};
			scalar data[2];
		};

		Vector() : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)) {}
		Vector(Vector&& other) : x(other.x), y(other.y)                 {}
		Vector(const Vector& other) : x(other.x), y(other.y)            {}

		explicit Vector(scalar s) : x(s), y(s)      {}
		Vector(scalar _x, scalar _y) : x(_x), y(_y) {}

		Vector operator=(Vector&& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		Vector operator=(const Vector& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		scalar& operator[](size_t i)             { ASSERT((i >= 0) && (i < 2)); return data[i]; }
        const scalar& operator[](size_t i) const { ASSERT((i >= 0) && (i < 2)); return data[i]; }

		template<typename _Scal>
		explicit Vector(Vector<_Scal, 2>&& other)      : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)) {}
		template<typename _Scal>
		explicit Vector(const Vector<_Scal, 2>& other) : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)) {}

		forceinline size_t size()    const { return 2; }
		forceinline scalar length()  const { return sqrt(dot(*this, *this)); }
		forceinline scalar length2() const { return dot(*this, *this); }
		forceinline scalar angle()   const { return std::atan2(y, x); }
	};

	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::up    = Vector<_Scalar, 2>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 1));
	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::down  = Vector<_Scalar, 2>(static_cast<_Scalar>( 0), static_cast<_Scalar>(-1));
	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::right = Vector<_Scalar, 2>(static_cast<_Scalar>( 1), static_cast<_Scalar>( 0));
	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::left  = Vector<_Scalar, 2>(static_cast<_Scalar>(-1), static_cast<_Scalar>( 0));
	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::one   = Vector<_Scalar, 2>(static_cast<_Scalar>(1));
	template<typename _Scalar>
	const Vector<_Scalar, 2> Vector<_Scalar, 2>::zero  = Vector<_Scalar, 2>(static_cast<_Scalar>(0));

	template<typename _Scalar>
	struct Vector<_Scalar, 3>
	{
		static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

		typedef _Scalar scalar;
		typedef Vector<_Scalar, 3> type;

		static const type up;
		static const type down;
		static const type right;
		static const type left;
		static const type forward;
		static const type backward;
		static const type one;
		static const type zero;

		union
		{
			struct
			{
				scalar x, y, z;
			};
			scalar data[3];
		};

		Vector() : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)), z(static_cast<scalar>(0)) {}
		Vector(Vector&& other) : x(other.x), y(other.y), z(other.z)                                {}
		Vector(const Vector& other) : x(other.x), y(other.y), z(other.z)                           {}

		explicit Vector(scalar s)               : x(s), y(s), z(s) {}
		Vector(scalar _x, scalar _y, scalar _z) : x(_x), y(_y), z(_z) {}

		Vector operator=(Vector&& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		Vector operator=(const Vector& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		scalar& operator[](size_t i)             { ASSERT((i >= 0) && (i < 3)); return data[i]; }
		const scalar& operator[](size_t i) const { ASSERT((i >= 0) && (i < 3)); return data[i]; }

		template<typename _Scal>
		explicit Vector(Vector<_Scal, 3>&& other)      : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)) {}
		template<typename _Scal>
		explicit Vector(const Vector<_Scal, 3>& other) : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)) {}

		forceinline size_t size()    const { return 3; }
		forceinline scalar length()  const { return sqrt(dot(*this, *this)); }
		forceinline scalar length2() const { return dot(*this, *this); }
	};

	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::up		  = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::down	  = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::right	  = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::left	  = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::forward  = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::backward = Vector<_Scalar, 3>(static_cast<_Scalar>( 0), static_cast<_Scalar>( 0), static_cast<_Scalar>( 0)); /* TODO: REQUIRE COORD SYSTEM */
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::one	  = Vector<_Scalar, 3>(static_cast<_Scalar>(1));
	template<typename _Scalar>
	const Vector<_Scalar, 3> Vector<_Scalar, 3>::zero	  = Vector<_Scalar, 3>(static_cast<_Scalar>(0));

	template<typename _Scalar>
	struct Vector<_Scalar, 4>
	{
		static_assert(std::is_arithmetic<_Scalar>::value, "Scalar must be a numeric type.");

		typedef _Scalar scalar;
		typedef Vector<_Scalar, 4> type;

		static const type one;
		static const type zero;

		union
		{
			struct
			{
				scalar x, y, z, w;
			};
			scalar data[4];
		};

		Vector() : x(static_cast<scalar>(0)), y(static_cast<scalar>(0)),
			z(static_cast<scalar>(0)), w(static_cast<scalar>(0))                     {}
		Vector(Vector&& other) : x(other.x), y(other.y), z(other.z), w(other.w)      {}
		Vector(const Vector& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

		explicit Vector(scalar s)                          : x(s), y(s), z(s), w(s) {}
		Vector(scalar _x, scalar _y, scalar _z, scalar _w) : x(_x), y(_y), z(_z), w(_w) {}

		Vector operator=(Vector&& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
			return *this;
		}

		Vector operator=(const Vector& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
			w = other.w;
			return *this;
		}

		scalar& operator[](size_t i)             { ASSERT((i >= 0) && (i < 4)); return data[i]; }
        const scalar& operator[](size_t i) const { ASSERT((i >= 0) && (i < 4)); return data[i]; }

		template<typename _Scal>
		explicit Vector(Vector<_Scal, 4>&& other)      : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)), w(static_cast<scalar>(other.w)) {}
		template<typename _Scal>
		explicit Vector(const Vector<_Scal, 4>& other) : x(static_cast<scalar>(other.x)), y(static_cast<scalar>(other.y)), z(static_cast<scalar>(other.z)), w(static_cast<scalar>(other.w)) {}

		forceinline size_t size()    const { return 4; }
		forceinline scalar length()  const { return sqrt(dot(*this, *this)); }
		forceinline scalar length2() const { return dot(*this, *this); }
	};

	template<typename _Scalar>
	const Vector<_Scalar, 4> Vector<_Scalar, 4>::one  = Vector<_Scalar, 4>(static_cast<_Scalar>(1));
	template<typename _Scalar>
	const Vector<_Scalar, 4> Vector<_Scalar, 4>::zero = Vector<_Scalar, 4>(static_cast<_Scalar>(0));

#pragma endregion
	/////////////////////////////////////////////// COMPUTE //////////////////////////////////////////////////////
#pragma region Vector math functions

	namespace detail
	{
#pragma region Sqrt
		template<typename _Scalar, size_t _Size>
		struct ComputeSqrt
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& v)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = std::sqrt(v.data[i]);
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeSqrt<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& v)
			{
				return std::sqrt(v.x);
			}
		};

		template<typename _Scalar>
		struct ComputeSqrt<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& v)
			{
				return Vector<_Scalar, 2>(
					std::sqrt(v.x),
					std::sqrt(v.y)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeSqrt<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& v)
			{
				return Vector<_Scalar, 3>(
					std::sqrt(v.x),
					std::sqrt(v.y),
					std::sqrt(v.z)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeSqrt<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& v)
			{
				return Vector<_Scalar, 4>(
					std::sqrt(v.x),
					std::sqrt(v.y),
					std::sqrt(v.z), 
					std::sqrt(v.w)
					);
			}
		};
#pragma endregion

#pragma region Dot
		template<typename _Scalar, size_t _Size>
		struct ComputeDot
		{
			static _Scalar compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				_Scalar result = static_cast<_Scalar>(0);
				for (size_t i = 0; i < _Size; i++)
				{
					result += a[i] * b[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeDot<_Scalar, 1>
		{
			static _Scalar compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x * b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeDot<_Scalar, 2>
		{
			static _Scalar compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return a.x * b.x + a.y * b.y;
			}
		};

		template<typename _Scalar>
		struct ComputeDot<_Scalar, 3>
		{
			static _Scalar compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return a.x * b.x + a.y * b.y + a.z * b.z;
			}
		};

		template<typename _Scalar>
		struct ComputeDot<_Scalar, 4>
		{
			static _Scalar compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
			}
		};
#pragma endregion

#pragma region Cross
		template<typename _Scalar, size_t _Size>
		struct ComputeCross
		{
			static _Scalar compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				static_assert(false, "Ill formed cross product.");
			}
		};


		template<typename _Scalar>
		struct ComputeCross<_Scalar, 3>
		{
			static _Scalar compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return _Scalar(
					(a.y * b.z) - (a.z * b.y),
					(a.z * b.x) - (a.x * b.z),
					(a.x * b.y) - (a.y * b.x)
				);
			}
		};
#pragma endregion

#pragma region Negate
		template<typename _Scalar, size_t _Size>
		struct ComputeNegate
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& v)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = -v.data[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeNegate<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& v)
			{
				return -v.x;
			}
		};

		template<typename _Scalar>
		struct ComputeNegate<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& v)
			{
				return Vector<_Scalar, 2>(-v.x, -v.y);
			}
		};

		template<typename _Scalar>
		struct ComputeNegate<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& v)
			{
				return Vector<_Scalar, 3>(-v.x, -v.y, -v.z);
			}
		};

		template<typename _Scalar>
		struct ComputeNegate<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& v)
			{
				return Vector<_Scalar, 4>(-v.x, -v.y, -v.z, -v.w);
			}
		};
#pragma endregion

#pragma region Add
		template<typename _Scalar, size_t _Size>
		struct ComputeAdd
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] + b[i];
				}
				return result;
			}

			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, _Scalar b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] + b;
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeAdd<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x + b.x;
			}

			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, _Scalar b)
			{
				return a.x + b;
			}
		};

		template<typename _Scalar>
		struct ComputeAdd<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<_Scalar, 2>(
					a.x + b.x,
					a.y + b.y
					);
			}

			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, _Scalar b)
			{
				return Vector<_Scalar, 2>(
					a.x + b,
					a.y + b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeAdd<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<_Scalar, 3>(
					a.x + b.x, 
					a.y + b.y, 
					a.z + b.z
					);
			}

			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, _Scalar b)
			{
				return Vector<_Scalar, 3>(
					a.x + b, 
					a.y + b, 
					a.z + b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeAdd<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<_Scalar, 4>(
					a.x + b.x,
					a.y + b.y, 
					a.z + b.z,
					a.w + b.w
					);
			}

			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, _Scalar b)
			{
				return Vector<_Scalar, 4>(
					a.x + b,
					a.y + b,
					a.z + b,
					a.w + b
					);
			}
		};
#pragma endregion

#pragma region Subtract
		template<typename _Scalar, size_t _Size>
		struct ComputeSub
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] - b[i];
				}
				return result;
			}

			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, _Scalar b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] - b;
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeSub<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x - b.x;
			}

			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, _Scalar b)
			{
				return a.x - b;
			}
		};

		template<typename _Scalar>
		struct ComputeSub<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<_Scalar, 2>(
					a.x - b.x,
					a.y - b.y
					);
			}

			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, _Scalar b)
			{
				return Vector<_Scalar, 2>(
					a.x - b,
					a.y - b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeSub<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<_Scalar, 3>(
					a.x - b.x, 
					a.y - b.y,
					a.z - b.z
					);
			}

			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, _Scalar b)
			{
				return Vector<_Scalar, 3>(
					a.x - b,
					a.y - b,
					a.z - b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeSub<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<_Scalar, 4>(
					a.x - b.x,
					a.y - b.y, 
					a.z - b.z,
					a.w - b.w
					);
			}

			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, _Scalar b)
			{
				return Vector<_Scalar, 4>(
					a.x - b,
					a.y - b,
					a.z - b, 
					a.w - b
					);
			}
		};
#pragma endregion

#pragma region Multiply
		template<typename _Scalar, size_t _Size>
		struct ComputeMult
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] * b[i];
				}
				return result;
			}

			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, _Scalar b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] * b;
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeMult<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x * b.x;
			}

			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, _Scalar b)
			{
				return a.x * b;
			}
		};

		template<typename _Scalar>
		struct ComputeMult<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<_Scalar, 2>(
					a.x * b.x,
					a.y * b.y
					);
			}

			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, _Scalar b)
			{
				return Vector<_Scalar, 2>(
					a.x * b,
					a.y * b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMult<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<_Scalar, 3>(
					a.x * b.x,
					a.y * b.y, 
					a.z * b.z
					);
			}

			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, _Scalar b)
			{
				return Vector<_Scalar, 3>(
					a.x * b,
					a.y * b,
					a.z * b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMult<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<_Scalar, 4>(
					a.x * b.x,
					a.y * b.y,
					a.z * b.z,
					a.w * b.w
					);
			}

			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, _Scalar b)
			{
				return Vector<_Scalar, 4>(
					a.x * b,
					a.y * b,
					a.z * b,
					a.w * b
					);
			}
		};
#pragma endregion

#pragma region Divide
		template<typename _Scalar, size_t _Size>
		struct ComputeDiv
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] / b[i];
				}
				return result;
			}

			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& a, _Scalar b)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] / b;
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeDiv<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x / b.x;
			}

			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& a, _Scalar b)
			{
				return a.x / b;
			}
		};

		template<typename _Scalar>
		struct ComputeDiv<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<_Scalar, 2>(
					a.x / b.x,
					a.y / b.y
					);
			}

			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& a, _Scalar b)
			{
				return Vector<_Scalar, 2>(
					a.x / b, 
					a.y / b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeDiv<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<_Scalar, 3>(
					a.x / b.x,
					a.y / b.y,
					a.z / b.z
					);
			}

			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& a, _Scalar b)
			{
				return Vector<_Scalar, 3>(
					a.x / b, 
					a.y / b,
					a.z / b
					);
			}
		};

		template<typename _Scalar>
		struct ComputeDiv<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<_Scalar, 4>(
					a.x / b.x,
					a.y / b.y, 
					a.z / b.z,
					a.w / b.w
					);
			}

			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& a, _Scalar b)
			{
				return Vector<_Scalar, 4>(
					a.x / b,
					a.y / b,
					a.z / b,
					a.w / b
					);
			}
		};
#pragma endregion

#pragma region Equate
		template<typename _Scalar, size_t _Size>
		struct ComputeEquate
		{
			static bool compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				for (size_t i = 0; i < _Size; i++)
				{
					if (a[i] != b[i])
					{
						return false;
					}
				}
				return true;
			}
		};

		template<typename _Scalar>
		struct ComputeEquate<_Scalar, 1>
		{
			static bool compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x == b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeEquate<_Scalar, 2>
		{
			static bool compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return a.x == b.x && a.y == b.y;
			}
		};

		template<typename _Scalar>
		struct ComputeEquate<_Scalar, 3>
		{
			static bool compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return a.x == b.x && a.y == b.y && a.z == b.z;
			}
		};

		template<typename _Scalar>
		struct ComputeEquate<_Scalar, 4>
		{
			static bool compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
			}
		};
#pragma endregion

#pragma region Greater
		template<typename _Scalar, size_t _Size>
		struct ComputeGreater
		{
			static Vector<bool, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<bool, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] > b[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeGreater<_Scalar, 1>
		{
			static Vector<bool, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x > b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeGreater<_Scalar, 2>
		{
			static Vector<bool, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<bool, 2>(
					a.x > b.x,
					a.y > b.y
					);
			}
		};

		template<typename _Scalar>
		struct ComputeGreater<_Scalar, 3>
		{
			static Vector<bool, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<bool, 3>(
					a.x > b.x,
					a.y > b.y,
					a.z > b.z
					);
			}
		};

		template<typename _Scalar>
		struct ComputeGreater<_Scalar, 4>
		{
			static Vector<bool, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<bool, 4>(
					a.x > b.x, 
					a.y > b.y,
					a.z > b.z,
					a.w > b.w
					);
			}
		};
#pragma endregion

#pragma region Less
		template<typename _Scalar, size_t _Size>
		struct ComputeLess
		{
			static Vector<bool, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<bool, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] < b[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeLess<_Scalar, 1>
		{
			static Vector<bool, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x < b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeLess<_Scalar, 2>
		{
			static Vector<bool, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<bool, 2>(
					a.x < b.x, 
					a.y < b.y
					);
			}
		};

		template<typename _Scalar>
		struct ComputeLess<_Scalar, 3>
		{
			static Vector<bool, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<bool, 3>(
					a.x < b.x,
					a.y < b.y, 
					a.z < b.z
					);
			}
		};

		template<typename _Scalar>
		struct ComputeLess<_Scalar, 4>
		{
			static Vector<bool, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<bool, 4>(
					a.x < b.x,
					a.y < b.y, 
					a.z < b.z,
					a.w < b.w
					);
			}
		};
#pragma endregion

#pragma region Greater or equal
		template<typename _Scalar, size_t _Size>
		struct ComputeGequal
		{
			static Vector<bool, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<bool, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] >= b[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeGequal<_Scalar, 1>
		{
			static Vector<bool, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x >= b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeGequal<_Scalar, 2>
		{
			static Vector<bool, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<bool, 2>(
					a.x >= b.x,
					a.y >= b.y
					);
			}
		};

		template<typename _Scalar>
		struct ComputeGequal<_Scalar, 3>
		{
			static Vector<bool, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<bool, 3>(
					a.x >= b.x,
					a.y >= b.y,
					a.z >= b.z
					);
			}
		};

		template<typename _Scalar>
		struct ComputeGequal<_Scalar, 4>
		{
			static Vector<bool, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<bool, 4>(
					a.x >= b.x,
					a.y >= b.y,
					a.z >= b.z,
					a.w >= b.w
					);
			}
		};
#pragma endregion

#pragma region Less or equal
		template<typename _Scalar, size_t _Size>
		struct ComputeLequal
		{
			static Vector<bool, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<bool, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = a[i] <= b[i];
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeLequal<_Scalar, 1>
		{
			static Vector<bool, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return a.x <= b.x;
			}
		};

		template<typename _Scalar>
		struct ComputeLequal<_Scalar, 2>
		{
			static Vector<bool, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<bool, 2>(
					a.x <= b.x,
					a.y <= b.y
					);
			}
		};

		template<typename _Scalar>
		struct ComputeLequal<_Scalar, 3>
		{
			static Vector<bool, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<bool, 3>(
					a.x <= b.x,
					a.y <= b.y,
					a.z <= b.z
					);
			}
		};

		template<typename _Scalar>
		struct ComputeLequal<_Scalar, 4>
		{
			static Vector<bool, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<bool, 4>(
					a.x <= b.x,
					a.y <= b.y, 
					a.z <= b.z, 
					a.w <= b.w
					);
			}
		};
#pragma endregion

#pragma region Compare
		template<typename _Scalar, size_t _Size>
		struct ComputeComp
		{
			static Vector<isi_s8, _Size> compute(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
			{
				Vector<bool, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = (a[i] < b[i]? -1 : (a[i] > b[i]? 1 : 0));
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeComp<_Scalar, 1>
		{
			static Vector<isi_s8, 1> compute(const Vector<_Scalar, 1>& a, const Vector<_Scalar, 1>& b)
			{
				return (a.x < b.x? -1 : (a.x > b.x? 1 : 0));
			}
		};

		template<typename _Scalar>
		struct ComputeComp<_Scalar, 2>
		{
			static Vector<isi_s8, 2> compute(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b)
			{
				return Vector<bool, 2>(
					(a.x < b.x? -1 : (a.x > b.x? 1 : 0)),
					(a.y < b.y? -1 : (a.y > b.y? 1 : 0))
					);
			}
		};

		template<typename _Scalar>
		struct ComputeComp<_Scalar, 3>
		{
			static Vector<isi_s8, 3> compute(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b)
			{
				return Vector<bool, 3>(
					(a.x < b.x? -1 : (a.x > b.x? 1 : 0)), 
					(a.y < b.y? -1 : (a.y > b.y? 1 : 0)),
					(a.z < b.z? -1 : (a.z > b.z? 1 : 0))
					);
			}
		};

		template<typename _Scalar>
		struct ComputeComp<_Scalar, 4>
		{
			static Vector<isi_s8, 4> compute(const Vector<_Scalar, 4>& a, const Vector<_Scalar, 4>& b)
			{
				return Vector<bool, 4>(
					(a.x < b.x? -1 : (a.x > b.x? 1 : 0)), 
					(a.y < b.y? -1 : (a.y > b.y? 1 : 0)), 
					(a.z < b.z? -1 : (a.z > b.z? 1 : 0)),
					(a.w < b.w? -1 : (a.w > b.w? 1 : 0))
					);
			}
		};
#pragma endregion

#pragma region Min
		template<typename _Scalar, size_t _Size>
		struct ComputeMin
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& v, const Vector<_Scalar, _Size>& min)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = std::min(v.data[i], min.data[i]);
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeMin<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& v, const Vector<_Scalar, 1>& min)
			{
				return std::min(v.x, min.x);
			}
		};

		template<typename _Scalar>
		struct ComputeMin<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& v, const Vector<_Scalar, 2>& min)
			{
				return Vector<_Scalar, 2>(
					std::min(v.x, min.x),
					std::min(v.y, min.y)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMin<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& v, const Vector<_Scalar, 3>& min)
			{
				return Vector<_Scalar, 3>(
					std::min(v.x, min.x),
					std::min(v.y, min.y),
					std::min(v.z, min.z)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMin<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& v, const Vector<_Scalar, 4>& min)
			{
				return Vector<_Scalar, 3>(
					std::min(v.x, min.x),
					std::min(v.y, min.y),
					std::min(v.z, min.z), 
					std::min(v.w, min.w)
					);
			}
		};
#pragma endregion
		
#pragma region Max
		template<typename _Scalar, size_t _Size>
		struct ComputeMax
		{
			static Vector<_Scalar, _Size> compute(const Vector<_Scalar, _Size>& v, const Vector<_Scalar, _Size>& max)
			{
				Vector<_Scalar, _Size> result;
				for (size_t i = 0; i < _Size; i++)
				{
					result[i] = std::max(v.data[i], max.data[i]);
				}
				return result;
			}
		};

		template<typename _Scalar>
		struct ComputeMax<_Scalar, 1>
		{
			static Vector<_Scalar, 1> compute(const Vector<_Scalar, 1>& v, const Vector<_Scalar, 1>& max)
			{
				return std::max(v.x, max.x);
			}
		};

		template<typename _Scalar>
		struct ComputeMax<_Scalar, 2>
		{
			static Vector<_Scalar, 2> compute(const Vector<_Scalar, 2>& v, const Vector<_Scalar, 2>& max)
			{
				return Vector<_Scalar, 2>(
					std::max(v.x, max.x),
					std::max(v.y, max.y)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMax<_Scalar, 3>
		{
			static Vector<_Scalar, 3> compute(const Vector<_Scalar, 3>& v, const Vector<_Scalar, 3>& max)
			{
				return Vector<_Scalar, 3>(
					std::max(v.x, max.x),
					std::max(v.y, max.y), 
					std::max(v.z, max.z)
					);
			}
		};

		template<typename _Scalar>
		struct ComputeMax<_Scalar, 4>
		{
			static Vector<_Scalar, 4> compute(const Vector<_Scalar, 4>& v, const Vector<_Scalar, 4>& max)
			{
				return Vector<_Scalar, 3>(
					std::max(v.x, max.x), 
					std::max(v.y, max.y), 
					std::max(v.z, max.z), 
					std::max(v.w, max.w)
					);
			}
		};
#pragma endregion
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator-(const Vector<_Scalar, _Size>& v)
	{
		return detail::ComputeNegate<_Scalar, _Size>::compute(v);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator+(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeAdd<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator-(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeSub<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator*(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeMult<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator/(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeDiv<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator+=(Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		a = detail::ComputeAdd<_Scalar, _Size>::compute(a, b);
		return a;
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator-=(Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		a = detail::ComputeSub<_Scalar, _Size>::compute(a, b);
		return a;
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator*=(Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		a = detail::ComputeMult<_Scalar, _Size>::compute(a, b);
		return a;
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> operator/=(Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		a = detail::ComputeDiv<_Scalar, _Size>::compute(a, b);
		return a;
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator+(const Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		return detail::ComputeAdd<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator-(const Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		return detail::ComputeSub<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator*(const Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		return detail::ComputeMult<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator*(_Scal2 a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeMult<_Scalar, _Size>::compute(b, a);
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator/(const Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		return detail::ComputeDiv<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
	}
	
	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator+=(Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		a = detail::ComputeAdd<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
		return a;
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator-=(Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		a = detail::ComputeSub<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
		return a;
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator*=(Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		a = detail::ComputeMult<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
		return a;
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline Vector<_Scalar, _Size> operator/=(Vector<_Scalar, _Size>& a, _Scal2 b)
	{
		a = detail::ComputeDiv<_Scalar, _Size>::compute(a, static_cast<_Scalar>(b));
		return a;
	}

	template<typename _Scalar, size_t _Size>
	forceinline bool operator==(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeEquate<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline bool operator!=(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return !(a == b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<bool, _Size> operator>(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeGreater<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<bool, _Size> operator<(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeLess<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<bool, _Size> operator>=(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeGequal<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<bool, _Size> operator<=(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeLequal<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<isi_s8, _Size> compare(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		detail::ComputeComp<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<isi_s8, _Size> min(const Vector<_Scalar, _Size>& v, const Vector<_Scalar, _Size>& min)
	{
		return detail::ComputeMin<_Scalar, _Size>::compute(v, min);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<isi_s8, _Size> max(const Vector<_Scalar, _Size>& v, const Vector<_Scalar, _Size>& max)
	{
		return detail::ComputeMax<_Scalar, _Size>::compute(v, max);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<isi_s8, _Size> clamp(const Vector<_Scalar, _Size>& v, const Vector<_Scalar, _Size>& min, const Vector<_Scalar, _Size>& max)
	{
		return detail::ComputeMin<_Scalar, _Size>::compute(
			detail::ComputeMax<_Scalar, _Size>::compute(v, max)
			, min);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> sqrt(const Vector<_Scalar, _Size>& v)
	{
		return detail::ComputeSqrt<_Scalar, _Size>::compute(v);
	}

	template<typename _Scalar, size_t _Size>
	forceinline _Scalar dot(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeDot<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> cross(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return detail::ComputeCross<_Scalar, _Size>::compute(a, b);
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> normalize(const Vector<_Scalar, _Size>& v)
	{
		return v / v.length();
	}

	template<typename _Scalar, size_t _Size>
	forceinline Vector<_Scalar, _Size> fast_normalize(const Vector<_Scalar, _Size>& v)
	{
		v * fast_inv_sqrt(dot(v, v));
	}

	template<typename _Scalar, size_t _Size>
	forceinline _Scalar distance2(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return (a - b).length2();
	}

	template<typename _Scalar, size_t _Size>
	forceinline _Scalar distance(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b)
	{
		return (a - b).length();
	}

	template<typename _Scalar, size_t _Size, typename _Scal2>
	forceinline _Scalar lerp(const Vector<_Scalar, _Size>& a, const Vector<_Scalar, _Size>& b, _Scal2 fract)
	{
		return a + (fract * (b - a));
	}

	template<typename _Scalar>
	forceinline Vector<_Scalar, 2> from_angle(_Scalar radians)
	{
		return Vector<_Scalar, 2>(std::cos(radians), std::sin(radians));
	}

	template<typename _Scalar>
	forceinline Vector<_Scalar, 2> rotate(const Vector<_Scalar, 2>& v, _Scalar radians)
	{
		return from_angle(v.angle() + radians) * v.length();
	}

	template<typename _Scalar, typename _Scal2>
	forceinline Vector<_Scalar, 2> slerp(const Vector<_Scalar, 2>& a, const Vector<_Scalar, 2>& b, _Scal2 fract)
	{
		return from_angle(lerp(a.angle(), b.angle(), fract));
	}

	template<typename _Scalar>
	forceinline Vector<_Scalar, 3> rotate(const Vector<_Scalar, 3>& v, _Scalar radians, const Vector<_Scalar, 3>& axis); /* TODO: REQUIRE QUATERNIONS */

	template<typename _Scalar, typename _Scal2>
	forceinline Vector<_Scalar, 3> slerp(const Vector<_Scalar, 3>& a, const Vector<_Scalar, 3>& b, _Scal2 fract); /* TODO: REQUIRE QUATERNIONS */

	template<size_t _Size>
	forceinline bool any(const Vector<bool, _Size>& v)
	{
		for (size_t i = 0; i < _Size; i++)
		{
			if (v.data[i])
			{
				return true;
			}
		}
		return false;
	}

	template<size_t _Size>
	forceinline bool all(const Vector<bool, _Size>& v)
	{
		bool result = true;
		for (size_t i = 0; i < _Size; i++)
		{
			if (!v.data[i])
			{
				return false;
			}
		}
		return true;
	}

#pragma endregion

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//																											//
	//												ALIAS														//
	//																											//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Type aliases

	typedef Vector<float, 1> Vec1;
	typedef Vector<float, 2> Vec2;
	typedef Vector<float, 3> Vec3;
	typedef Vector<float, 4> Vec4;
	typedef Vector<double, 1> dVec1;
	typedef Vector<double, 2> dVec2;
	typedef Vector<double, 3> dVec3;
	typedef Vector<double, 4> dVec4;
	typedef Vector<int, 1> iVec1;
	typedef Vector<int, 2> iVec2;
	typedef Vector<int, 3> iVec3;
	typedef Vector<int, 4> iVec4;
	typedef Vector<bool, 1> bVec1;
	typedef Vector<bool, 2> bVec2;
	typedef Vector<bool, 3> bVec3;
	typedef Vector<bool, 4> bVec4;

#pragma endregion
}
