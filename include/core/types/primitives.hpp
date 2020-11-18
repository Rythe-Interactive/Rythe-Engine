#pragma once
#ifndef _MSC_VER
#include <cstdint> //linux std::(u)int(8-64)_t
#else
#include <cstddef> //msc   std::(u)int(8-64)_t
#endif
#include <vector>  //      std::vector<T>
#include <string>
#include <string_view>

/**
 * @file primitives.hpp
 */

namespace legion::core
{
	using uint8 = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;
	using uint_max = std::uintmax_t;

	using int8 = std::int8_t;
	using int16 = std::int16_t;
	using int32 = std::int32_t;
	using int64 = std::int64_t;
	using int_max = std::intmax_t;

	using size_type = std::size_t;
	using index_type = std::size_t;

	using f32 = float;
	using f64 = double;

	using f80 = long double;
	
	using time32 = float;
	using time64 = double;

	using cstring = const char*;

	using uint = uint32;

	using byte = uint8;
    using byte_vec = std::vector<byte>;

    using bitfield8 = byte;
    using bitfield16 = uint16;
    using bitfield32 = uint32;
    using bitfield64 = uint64;

	using priority_type = uint8;
#define default_priority 0
#define PRIORITY_MAX UCHAR_MAX

	using id_type = uint_max;

#define invalid_id 0

	using hours = time32;
	using minutes = time32;
	using seconds = time32;
	using milli_seconds = time32;
	using micro_seconds = time32;

	using fast_time = time32;
	using fast_hours = fast_time;
	using fast_minutes = fast_time;
	using fast_seconds = fast_time;
	using fast_milli_seconds = fast_time;
	using fast_micro_seconds = fast_time;
	using fast_nano_seconds = fast_time;

	using precise_time = time64;
	using precise_hours = precise_time;
	using precise_minutes = precise_time;
	using precise_seconds = precise_time;
	using precise_milli_seconds = precise_time;
	using precise_micro_seconds = precise_time;
	using precise_nano_seconds = precise_time;
}
