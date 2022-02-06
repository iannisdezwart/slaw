#ifndef SLAW_SIMD_H
#define SLAW_SIMD_H

#include "types.hpp"
#include "math.hpp"

// Below are the SIMD types that are supported by WebAssembly.

typedef i8 i8x16 __attribute__((__vector_size__(16)));
typedef i16 i16x8 __attribute__((__vector_size__(16)));
typedef i32 i32x4 __attribute__((__vector_size__(16)));
typedef i64 i64x2 __attribute__((__vector_size__(16)));

typedef f32 f32x4 __attribute__((__vector_size__(16)));
typedef f64 f64x2 __attribute__((__vector_size__(16)));

namespace slaw
{
/**
 * A compile-time function that returns true if the given type is a SIMD
 * vector type.
 */
template <typename T>
constexpr bool
is_simd_vector()
{
	if constexpr (is_same<T, i8x16>() || is_same<T, i16x8>()
		|| is_same<T, i32x4>() || is_same<T, i64x2>()
		|| is_same<T, f32x4>() || is_same<T, f64x2>())
	{
		return true;
	}

	return false;
}

/**
 * A compile-time function that returns the number of elements in the given
 * SIMD vector type.
 */
template <typename T>
constexpr usize
simd_vector_size()
{
	if constexpr (is_same<T, i8x16>())
	{
		return 16;
	}

	if constexpr (is_same<T, i16x8>())
	{
		return 8;
	}

	if constexpr (is_same<T, i32x4>() || is_same<T, f32x4>())
	{
		return 4;
	}

	if constexpr (is_same<T, i64x2>() || is_same<T, f64x2>())
	{
		return 2;
	}

	return 0;
}

namespace simd
{

/**
 * A compile-time structure that holds the 128-bit SIMD vector type that
 * corresponds to a given type.
 */
template <typename T>
struct simd_vector_of {};

template <> struct simd_vector_of<i8> { using type = i8x16; };
template <> struct simd_vector_of<i16> { using type = i16x8; };
template <> struct simd_vector_of<i32> { using type = i32x4; };
template <> struct simd_vector_of<i64> { using type = i64x2; };
template <> struct simd_vector_of<f32> { using type = f32x4; };
template <> struct simd_vector_of<f64> { using type = f64x2; };

/**
 * A compile-time structure that holds the element type of a given SIMD
 * vector type.
 */
template <typename T>
struct simd_element_type_of {};

template <> struct simd_element_type_of<i8x16> { using type = i8; };
template <> struct simd_element_type_of<i16x8> { using type = i16; };
template <> struct simd_element_type_of<i32x4> { using type = i32; };
template <> struct simd_element_type_of<i64x2> { using type = i64; };
template <> struct simd_element_type_of<f32x4> { using type = f32; };
template <> struct simd_element_type_of<f64x2> { using type = f64; };

/**
 * Performs an element-wise maximum operation on two SIMD vectors.
 * The output of each element is the maximum of the corresponding
 * elements of the two input vectors.
 */
template <typename T>
constexpr T
max(const T &a, const T &b)
{
	static_assert(simd_vector_size<T>() != 0,
		"Type is not a SIMD vector type.");

	// We can't use `__builtin_elementwise_ax` because it isn't fully
	// supported in recent versions of clang, and it won't work at
	// compile-time. We will do a dumb hard-coded maximum operation
	// on each element instead. This will be optimised down to a single
	// vector instruction by the compiler.
	// https://godbolt.org/z/4zrzs573E

	if constexpr (simd_vector_size<T>() == 16)
	{
		return {
			slaw::max(a[0], b[0]),   slaw::max(a[1], b[1]),
			slaw::max(a[2], b[2]),   slaw::max(a[3], b[3]),
			slaw::max(a[4], b[4]),   slaw::max(a[5], b[5]),
			slaw::max(a[6], b[6]),   slaw::max(a[7], b[7]),
			slaw::max(a[8], b[8]),   slaw::max(a[9], b[9]),
			slaw::max(a[10], b[10]), slaw::max(a[11], b[11]),
			slaw::max(a[12], b[12]), slaw::max(a[13], b[13]),
			slaw::max(a[14], b[14]), slaw::max(a[15], b[15])
		};
	}

	if constexpr (simd_vector_size<T>() == 8)
	{
		return {
			slaw::max(a[0], b[0]), slaw::max(a[1], b[1]),
			slaw::max(a[2], b[2]), slaw::max(a[3], b[3]),
			slaw::max(a[4], b[4]), slaw::max(a[5], b[5]),
			slaw::max(a[6], b[6]), slaw::max(a[7], b[7])
		};
	}

	if constexpr (simd_vector_size<T>() == 4)
	{
		return {
			slaw::max(a[0], b[0]), slaw::max(a[1], b[1]),
			slaw::max(a[2], b[2]), slaw::max(a[3], b[3])
		};
	}

	if constexpr (simd_vector_size<T>() == 2)
	{
		return {
			slaw::max(a[0], b[0]), slaw::max(a[1], b[1])
		};
	}
}

/**
 * Performs an element-wise miniumum operation on two SIMD vectors.
 * The output of each element is the minimum of the corresponding
 * elements of the two input vectors.
 */
template <typename T>
constexpr T
min(const T &a, const T &b)
{
	static_assert(simd_vector_size<T>() != 0,
		"Type is not a SIMD vector type.");

	// We can't use `__builtin_elementwise_min` because it isn't fully
	// supported in recent versions of clang, and it won't work at
	// compile-time. We will do a dumb hard-coded miniumum operation
	// on each element instead. This will be optimised down to a single
	// vector instruction by the compiler.
	// https://godbolt.org/z/q6snrf5P9

	if constexpr (simd_vector_size<T>() == 16)
	{
		return {
			slaw::min(a[0], b[0]),   slaw::min(a[1], b[1]),
			slaw::min(a[2], b[2]),   slaw::min(a[3], b[3]),
			slaw::min(a[4], b[4]),   slaw::min(a[5], b[5]),
			slaw::min(a[6], b[6]),   slaw::min(a[7], b[7]),
			slaw::min(a[8], b[8]),   slaw::min(a[9], b[9]),
			slaw::min(a[10], b[10]), slaw::min(a[11], b[11]),
			slaw::min(a[12], b[12]), slaw::min(a[13], b[13]),
			slaw::min(a[14], b[14]), slaw::min(a[15], b[15])
		};
	}

	if constexpr (simd_vector_size<T>() == 8)
	{
		return {
			slaw::min(a[0], b[0]), slaw::min(a[1], b[1]),
			slaw::min(a[2], b[2]), slaw::min(a[3], b[3]),
			slaw::min(a[4], b[4]), slaw::min(a[5], b[5]),
			slaw::min(a[6], b[6]), slaw::min(a[7], b[7])
		};
	}

	if constexpr (simd_vector_size<T>() == 4)
	{
		return {
			slaw::min(a[0], b[0]), slaw::min(a[1], b[1]),
			slaw::min(a[2], b[2]), slaw::min(a[3], b[3])
		};
	}

	if constexpr (simd_vector_size<T>() == 2)
	{
		return {
			slaw::min(a[0], b[0]), slaw::min(a[1], b[1])
		};
	}
}

/**
 * Sums the elements of a SIMD vector.
 */
template <typename T>
constexpr typename simd_element_type_of<T>::type
sum(T v)
{
	static_assert(simd_vector_size<T>() != 0,
		"Type is not a SIMD vector type.");

	if constexpr (simd_vector_size<T>() == 16)
	{
		return v[0] + v[1] + v[2] + v[3] + v[4] + v[5] + v[6] + v[7]
			+ v[8] + v[9] + v[10] + v[11] + v[12] + v[13] + v[14]
			+ v[15];
	}

	if constexpr (simd_vector_size<T>() == 8)
	{
		return v[0] + v[1] + v[2] + v[3] + v[4] + v[5] + v[6] + v[7];
	}

	if constexpr (simd_vector_size<T>() == 4)
	{
		return v[0] + v[1] + v[2] + v[3];
	}

	if constexpr (simd_vector_size<T>() == 2)
	{
		return v[0] + v[1];
	}
}
}; // namespace slaw::simd
}; // namespace slaw

#endif