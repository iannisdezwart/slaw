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
 * A compile-time function that returns true if the type is an integer.
 */
template <typename T>
constexpr bool
is_integer()
{
	if constexpr (is_same<T, u8>() || is_same<T, i8>()
		|| is_same<T, u16>() || is_same<T, i16>()
		|| is_same<T, u32>() || is_same<T, i32>()
		|| is_same<T, u64>() || is_same<T, i64>())
	{
		return true;
	}

	return false;
}

/**
 * A compile-time function that returns true if the type is a floating-point.
 */
template <typename T>
constexpr bool
is_float()
{
	if constexpr (is_same<T, f32>() || is_same<T, f64>())
	{
		return true;
	}

	return false;
}

/**
 * A compile-time function that returns true if the type is a signed integer.
 * Any other type returns false.
 */
template <typename T>
constexpr bool
is_signed_integer()
{
	if constexpr (is_same<T, i8>() || is_same<T, i16>()
		|| is_same<T, i32>() || is_same<T, i64>())
	{
		return true;
	}

	return false;
}

/**
 * A compile-time function that returns true if the type is an unsigned integer.
 * Any other type returns false.
 */
template <typename T>
constexpr bool
is_unsigned_integer()
{
	if constexpr (is_same<T, u8>() || is_same<T, u16>()
		|| is_same<T, u32>() || is_same<T, u64>())
	{
		return true;
	}

	return false;
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
	static_assert(is_integer<T>() || is_float<T>(),
		"Type is not an integer or floating point type.");

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
}

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
	static_assert(is_integer<T>() || is_float<T>(),
		"Type is not an integer or floating point type.");

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
}

/**
 * Comparison function that returns true if the first value is less than the
 * second value.
 */
template <typename T>
constexpr bool
less(T a, T b)
{
	return a < b;
}

/**
 * Comparison function that returns true if the first value is bigger than the
 * second value.
 */
template <typename T>
constexpr bool
greater(T a, T b)
{
	return a > b;
}

namespace detail
{
// It seems like the only way to convert the raw bytes of a floating
// point number to an integer in C++ is to use bit_cast.
// Unions and the Quake III-style float-to-int conversion don't work
// at compile time.
// We will create convienence functions for converting to and from
// floating point types.

/**
 * Function that converts the raw bytes of 32-bit floating point value to an
 * integer.
 */
constexpr i32
interpret_float_as_int(f32 value)
{
	return __builtin_bit_cast(i32, value);
}

/**
 * Function that converts the raw bytes of 64-bit floating point value to an
 * integer.
 */
constexpr i64
interpret_float_as_int(f64 value)
{
	return __builtin_bit_cast(i64, value);
}

/**
 * Function that converts the raw bytes of a 32-bit integer to a floating
 * point value.
 */
constexpr f32
interpret_int_as_float(i32 value)
{
	return __builtin_bit_cast(f32, value);
}

/**
 * Function that converts the raw bytes of a 64-bit integer to a floating
 * point value.
 */
constexpr f64
interpret_int_as_float(i64 value)
{
	return __builtin_bit_cast(f64, value);
}

/**
 * Function that returns the sign of a floating point value.
 * If positive, returns 0, if negative, returns 1.
 */
constexpr bool
stored_sign(f32 value)
{
	return interpret_float_as_int(value) & 0x80000000;
}

/**
 * Function that returns the sign of a floating point value.
 * If positive, returns 0, if negative, returns 1.
 */
constexpr bool
stored_sign(f64 value)
{
	return interpret_float_as_int(value) & 0x8000000000000000;
}

/**
 * Function that returns the exponent that is stored in the IEEE754
 * representation of a 32-bit floating point number.
 */
constexpr i32
stored_exponent(f32 value)
{
	i32 bits = interpret_float_as_int(value);

	// Shift the exponent bits to the right.

	i32 exp = bits >> 23;

	// Mask off the sign bit, so we are just left with the exponent.

	exp &= 0xFF;

	// According to the IEEE754 single precision floating point standard,
	// the exponent is biased by 127.
	// So we need to subtract 127 from the exponent to get the real
	// exponent.

	return exp - 127;
}

/**
 * Function that returns the exponent that is stored in the IEEE754
 * representation of a 64-bit floating point number.
 */
constexpr i32
stored_exponent(f64 value)
{
	i64 bits = interpret_float_as_int(value);

	// Shift the exponent bits to the right.

	i32 exp = bits >> 52;

	// Mask off the sign bit, so we are just left with the exponent.

	exp &= 0x7FF;

	// According to the IEEE754 double precision floating point standard,
	// the exponent is biased by 1023.
	// So we need to subtract 1023 from the exponent to get the real
	// exponent.

	return exp - 1023;
}

/**
 * Function that returns the mantissa that is stored in the IEEE754
 * representation of a 32-bit floating point number.
 */
constexpr i32
stored_mantissa(f32 value)
{
	i32 bits = interpret_float_as_int(value);

	// Mask off the exponent and sign bits.

	return bits & 0x007FFFFF;
}

/**
 * Function that returns the mantissa that is stored in the IEEE754
 * representation of a 64-bit floating point number.
 */
constexpr i64
stored_mantissa(f64 value)
{
	i64 bits = interpret_float_as_int(value);

	// Mask off the exponent and sign bits.

	return bits & 0x000FFFFFFFFFFFFF;
}

/**
 * Function that sets the exponent of a 32-bit floating point number to 0.
 */
constexpr f32
clear_exponent(f32 value)
{
	// Get the raw bytes of the floating point number.

	i32 bits = interpret_float_as_int(value);

	// Mask off the exponent bits and set it to 127, which represents 0.

	bits &= 0b1'00000000'11111111111111111111111;
	bits |= 0b0'01111111'00000000000000000000000;

	// Return the new floating point number.

	return interpret_int_as_float(bits);
}

/**
 * Function that sets the exponent of a 64-bit floating point number to 0.
 */
constexpr f64
clear_exponent(f64 value)
{
	// Get the raw bytes of the floating point number.

	i64 bits = interpret_float_as_int(value);

	// Mask off the exponent bits and set it to 1023, which represents 0.

	bits &= 0x801FFFFFFFFFFFFF; // 0b1'000...'111...
	bits |= 0x3FF0000000000000; // 0b0'0111...'000...

	// Return the new floating point number.

	return interpret_int_as_float(bits);
}
}; // namespace slaw::detail

// Infinity for a 32-bit floating point type.
const constexpr f32 Infinity32 =
	detail::interpret_int_as_float((i32) 0x7F800000);

// Infinity for a 64-bit floating point type.
const constexpr f64 Infinity64 =
	detail::interpret_int_as_float((i64) 0x7FF0000000000000);

/**
 * Infinity for a given floating point type.
 */
template <typename T>
constexpr T
Infinity()
{
	static_assert(is_float<T>(), "Type is not a floating point type.");

	if constexpr (is_same<T, f32>())
	{
		return Infinity32;
	}

	if constexpr (is_same<T, f64>())
	{
		return Infinity64;
	}
}

/**
 * Function that checks if a 32-bit floating point number is not a number.
 */
constexpr bool
is_nan(f32 value)
{
	return detail::stored_exponent(value) == 128
		&& detail::stored_mantissa(value) != 0;
}

/**
 * Function that checks if a 64-bit floating point number is not a number.
 */
constexpr bool
is_nan(f64 value)
{
	return detail::stored_exponent(value) == 1024
		&& detail::stored_mantissa(value) != 0;
}

// The lowest positive value for a 32-bit floating point type.
const constexpr f32 Epsilon32 = 1.1920928955078125e-07f;

// The lowest positive value for a 64-bit floating point type.
const constexpr f64 Epsilon64 = 2.220446049250313e-16;

/**
 * The lowest positive value for a given floating point type.
 * Throws an error if the type is not a floating point type.
 */
template <typename T>
constexpr T
Epsilon()
{
	static_assert(is_float<T>(), "Type is not a floating point type.");

	if constexpr (is_same<T, f32>())
	{
		return Epsilon32;
	}

	if constexpr (is_same<T, f64>())
	{
		return Epsilon64;
	}
}

// An NaN for a 32-bit floating point type.
// Note that equality comparisons with NaN always resolve to false.
const constexpr f32 NaN32 =
	detail::interpret_int_as_float((i32) 0x7FC00000);

// An NaN for a 64-bit floating point type.
// Note that equality comparisons with NaN always resolve to false.
const constexpr f64 NaN64 =
	detail::interpret_int_as_float((i64) 0x7FF8000000000000);
}; // namespace slaw

#endif