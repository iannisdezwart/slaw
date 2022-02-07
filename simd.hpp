#ifndef SLAW_SIMD_H
#define SLAW_SIMD_H

#include "types.hpp"
#include "math.hpp"
#include "util.hpp"

// Below are the SIMD types that are supported by WebAssembly.

typedef i8 i8x16 __attribute__((__vector_size__(16)));
typedef u8 u8x16 __attribute__((__vector_size__(16)));
typedef i16 i16x8 __attribute__((__vector_size__(16)));
typedef u16 u16x8 __attribute__((__vector_size__(16)));
typedef i32 i32x4 __attribute__((__vector_size__(16)));
typedef u32 u32x4 __attribute__((__vector_size__(16)));
typedef i64 i64x2 __attribute__((__vector_size__(16)));
typedef u64 u64x2 __attribute__((__vector_size__(16)));

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
namespace detail
{
/**
 * A compile-time structure that holds the 128-bit SIMD vector type that
 * corresponds to a given type.
 */
template <typename T>
struct simd_vector_of_impl {};

template <> struct simd_vector_of_impl<i8> { using type = i8x16; };
template <> struct simd_vector_of_impl<i16> { using type = i16x8; };
template <> struct simd_vector_of_impl<i32> { using type = i32x4; };
template <> struct simd_vector_of_impl<i64> { using type = i64x2; };
template <> struct simd_vector_of_impl<f32> { using type = f32x4; };
template <> struct simd_vector_of_impl<f64> { using type = f64x2; };

/**
 * A compile-time structure that holds the element type of a given SIMD
 * vector type.
 */
template <typename T>
struct simd_element_type_of_impl {};

template <> struct simd_element_type_of_impl<i8x16> { using type = i8; };
template <> struct simd_element_type_of_impl<i16x8> { using type = i16; };
template <> struct simd_element_type_of_impl<i32x4> { using type = i32; };
template <> struct simd_element_type_of_impl<i64x2> { using type = i64; };
template <> struct simd_element_type_of_impl<f32x4> { using type = f32; };
template <> struct simd_element_type_of_impl<f64x2> { using type = f64; };
}; // namespace detail

/**
 * A compile-time macro that resolves into the 128-bit SIMD vector type that
 * corresponds to a given type.
 */
template <typename T>
using simd_vector_of = typename detail::simd_vector_of_impl<T>::type;

/**
 * A compile-time macro that resolves into the element type of a given SIMD
 * vector type.
 */
template <typename T>
using simd_element_type_of =
	typename detail::simd_element_type_of_impl<T>::type;

/**
 * Performs an element-wise user specified operation on an SIMD vector.
 * Each element goes through the user specified operation and the result
 * is stored in the output vector.
 */
template <typename T>
constexpr T
each(const T &v, simd_element_type_of<T> (*op)(simd_element_type_of<T>))
{
	static_assert(simd_vector_size<T>() != 0,
		"Type is not a SIMD vector type.");

	if constexpr (simd_vector_size<T>() == 16)
	{
		return {
			op(v[0]),  op(v[1]),  op(v[2]),  op(v[3]),
			op(v[4]),  op(v[5]),  op(v[6]),  op(v[7]),
			op(v[8]),  op(v[9]),  op(v[10]), op(v[11]),
			op(v[12]), op(v[13]), op(v[14]), op(v[15])
		};
	}

	if constexpr (simd_vector_size<T>() == 8)
	{
		return {
			op(v[0]), op(v[1]), op(v[2]), op(v[3]),
			op(v[4]), op(v[5]), op(v[6]), op(v[7])
		};
	}

	if constexpr (simd_vector_size<T>() == 4)
	{
		return { op(v[0]), op(v[1]), op(v[2]), op(v[3]) };
	}

	if constexpr (simd_vector_size<T>() == 2)
	{
		return { op(v[0]), op(v[1]) };
	}
}

/**
 * Performs an element-wise user specified operation on two SIMD vectors.
 * All corresponding elements go through the user specified operation and
 * the result is stored in the output vector.
 */
template <typename T>
constexpr T
each(const T &a, const T &b, simd_element_type_of<T> (*op)(
	simd_element_type_of<T>, simd_element_type_of<T>))
{
	static_assert(simd_vector_size<T>() != 0,
		"Type is not a SIMD vector type.");

	if constexpr (simd_vector_size<T>() == 16)
	{
		return {
			op(a[0],  b[0]),  op(a[1],  b[1]),
			op(a[2],  b[2]),  op(a[3],  b[3]),
			op(a[4],  b[4]),  op(a[5],  b[5]),
			op(a[6],  b[6]),  op(a[7],  b[7]),
			op(a[8],  b[8]),  op(a[9],  b[9]),
			op(a[10], b[10]), op(a[11], b[11]),
			op(a[12], b[12]), op(a[13], b[13]),
			op(a[14], b[14]), op(a[15], b[15])
		};
	}

	if constexpr (simd_vector_size<T>() == 8)
	{
		return {
			op(a[0], b[0]), op(a[1], b[1]),
			op(a[2], b[2]), op(a[3], b[3]),
			op(a[4], b[4]), op(a[5], b[5]),
			op(a[6], b[6]), op(a[7], b[7])
		};
	}

	if constexpr (simd_vector_size<T>() == 4)
	{
		return {
			op(a[0], b[0]), op(a[1], b[1]),
			op(a[2], b[2]), op(a[3], b[3])
		};
	}

	if constexpr (simd_vector_size<T>() == 2)
	{
		return { op(a[0], b[0]), op(a[1], b[1]) };
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
	return each(a, b, slaw::min);
}

/**
 * Performs an element-wise maximum operation on two SIMD vectors.
 * The output of each element is the maximum of the corresponding
 * elements of the two input vectors.
 */
template <typename T>
constexpr T
max(const T &a, const T &b)
{
	return each(a, b, slaw::max);
}

/**
 * Performs an element-wise absolute value operation on a SIMD vector.
 * The output of each element is the absolute value of the corresponding
 * element of the input vector.
 */
template <typename T>
constexpr T
abs(const T &v)
{
	return each(v, slaw::abs);
}

/**
 * Performs an element-wise negation operation on a SIMD vector.
 * The output of each element is the corresponding element of the input
 * vector multiplied by -1.
 */
template <typename T>
constexpr T
neg(const T &v)
{
	return each(v, [](T n) { return -n; });
}

/**
 * Performs an element-wise rounding operation on a SIMD vector.
 * Each element is rounded to the nearest integer.
 */
template <typename T>
constexpr T
round(const T &v)
{
	static_assert(is_float<simd_element_type_of<T>>(),
		"SIMD vector does not hold floating-point types.");

	return each(v, slaw::round);
}

/**
 * Performs an element-wise ceiling operation on a SIMD vector.
 * Each element is rounded up to the nearest integer.
 */
template <typename T>
constexpr T
ceil(const T &v)
{
	static_assert(is_float<simd_element_type_of<T>>(),
		"SIMD vector does not hold floating-point types.");

	return each(v, slaw::ceil);
}

/**
 * Performs an element-wise floor operation on a SIMD vector.
 * Each element is rounded down to the nearest integer.
 */
template <typename T>
constexpr T
floor(const T &v)
{
	static_assert(is_float<simd_element_type_of<T>>(),
		"SIMD vector does not hold floating-point types.");

	return each(v, slaw::floor);
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