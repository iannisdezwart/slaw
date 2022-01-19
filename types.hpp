#ifndef SLAW_TYPES_H
#define SLAW_TYPES_H

// The following type definitions define the integer types used throughout the
// slaw library.
// The types are in the form `xy`, where `x` is the signedness of the type,
// and `y` is the size.
// 'i' is used for signed integers, and 'u' is used for unsigned integers.
// The four valid sizes are 8, 16, 32, and 64.

typedef unsigned char u8;
typedef signed char i8;
typedef unsigned short u16;
typedef signed short i16;
typedef unsigned int u32;
typedef signed int i32;
typedef unsigned long long u64;
typedef signed long long i64;

// The following type definitions define the floating-point types used
// throughout the slaw library.
// The types are in the form `fy`, where `y` is the size of the type.
// The two valid sizes are 32 and 64.

typedef float f32;
typedef double f64;

// The following type definitions define the size types used throughout the
// slaw library.
// `usize` is used for unsigned integers,
// and `isize` is used for signed integers.
// Due to the 32-bit nature of WebAssembly, these types are always 32 bits.

typedef u32 usize;
typedef i32 isize;

namespace slaw::detail
{
/**
 * Helper struct for `slaw::is_same<T1, T2>()`.
 * Is used because partial template specialisation is not allowed for functions
 * in C++. It is allowed on structs, however.
 */
template <typename T1, typename T2>
struct is_same_helper
{
	static const bool value = false;
};

template <typename T>
struct is_same_helper<T, T>
{
	static const bool value = true;
};

// A huge value which can be used to represent infinity.
const constexpr double huge_value = 1.7976931348623158e+308 * 2;
}; // namespace slaw::detail

namespace slaw
{
/**
 * A compile-time function that checks if two given types are the same.
 * If they are, returns true.
 */
template <typename T1, typename T2>
constexpr bool
is_same()
{
	return detail::is_same_helper<T1, T2>::value;
}

/**
 * A compile-time function that returns true if the type is signed.
 * Throws a compile-time error if the type is not a valid integer type.
 */
template <typename T>
constexpr bool
is_signed()
{
	if constexpr (is_same<T, u8>() || is_same<T, u16>()
		|| is_same<T, u32>() || is_same<T, u64>())
	{
		return false;
	}
	if constexpr (is_same<T, i8>() || is_same<T, i16>()
		|| is_same<T, i32>() || is_same<T, i64>())
	{
		return true;
	}

	throw "Type is not an integer.";
}

/**
 * A compile-time function that returns true if the type is unsigned.
 * Throws a compile-time error if the type is not a valid integer type.
 */
template <typename T>
constexpr bool
is_unsigned()
{
	return !is_signed<T>();
}

/**
 * A compile-time function that returns the minimum value that a given type
 * can hold.
 * Throws a compile-time error if the type is not a valid integer or floating
 * point type.
 */
template <typename T>
constexpr T
min_value()
{
	if constexpr (is_same<T, u8>() || is_same<T, u16>()
		|| is_same<T, u32>() || is_same<T, u64>())
	{
		return 0;
	}

	if constexpr (is_same<T, i8>() || is_same<T, i16>()
		|| is_same<T, i32>() || is_same<T, i64>())
	{
		// We return 0b10000..., which is the minimum value for a
		// signed integer in two's complement.

		return ((T) 1) << (sizeof(T) * 8 - 1);
	}

	if constexpr (is_same<T, f32>())
	{
		return -3.402823466e+38f;
	}

	if constexpr (is_same<T, f64>())
	{
		return -1.7976931348623158e+308;
	}

	throw "Type is not an integer or floating point type.";
}

// Infinity for a 32-bit floating point type.
const constexpr f32 Infinity32 = detail::huge_value;

// Infinity for a 64-bit floating point type.
const constexpr f64 Infinity64 = detail::huge_value;

// Infinity for a 64-bit floating point type.
const constexpr f64 Infinity = Infinity64;

/**
 * A compile-time function that returns the maximum value that a given type
 * can hold.
 * Throws a compile-time error if the type is not a valid integer or floating
 * point type.
 */
template <typename T>
constexpr T
max_value()
{
	if constexpr (is_same<T, u8>() || is_same<T, u16>()
		|| is_same<T, u32>() || is_same<T, u64>())
	{
		// We return 0b1111..., which is the maximum value for an
		// unsigned integer in two's complement.

		return ~min_value<T>();
	}

	if constexpr (is_same<T, i8>() || is_same<T, i16>()
		|| is_same<T, i32>() || is_same<T, i64>())
	{
		// We return 0b01111..., which is the maximum value for a
		// signed integer in two's complement.

		return ~min_value<T>();
	}

	if constexpr (is_same<T, f32>())
	{
		return 3.402823466e+38f;
	}

	if constexpr (is_same<T, f64>())
	{
		return 1.7976931348623158e+308;
	}

	throw "Type is not an integer or floating point type.";
}
}; // namespace slaw

#endif