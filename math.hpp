#ifndef SLAW_MATH_H
#define SLAW_MATH_H

namespace slaw
{
/**
 * Returns the minimum of two values.
 */
template <typename T>
constexpr inline T
min(T a, T b)
{
	// This will be optimised away into a single instruction,
	// so we don't have to optimise this any further.
	// https://godbolt.org/z/EY4K91eMK

	if (a < b)
	{
		return a;
	}

	return b;
}

/**
 * Returns the maximum of two values.
 */
template <typename T>
constexpr inline T
max(T a, T b)
{
	// This will be optimised away into a single instruction,
	// so we don't have to optimise this any further.
	// https://godbolt.org/z/EY4K91eMK

	if (a > b)
	{
		return a;
	}

	return b;
}

/**
 * Returns the absolute value of a number.
 */
template <typename T>
constexpr inline T
abs(T n)
{
	if constexpr (is_unsigned<T>())
	{
		// If the type is unsigned, just return the value.

		return n;
	}

	// If the type is signed, we return the absolute value.

	// This doesn't have to be optimised further, because the compiler
	// will optimise this as much as possible.
	// I tested a couple of different implementation for the `abs` function
	// on Godbolt's compiler explorer, and all of them were optimised to
	// the same WASM code when using `-O1` or higher.
	// https://godbolt.org/z/fsjYvr6db

	if (n < 0)
	{
		return -n;
	}

	return n;
}

/**
 * Returns the popcount of a number.
 * This is defined as the number of bits that are set to 1.
 */
template <typename T>
constexpr inline T
popcnt(T n)
{
	// This will be optimised away into a single instruction for 32 and 64
	// bit integers, and two instructions for 8 and 16 bit integers,
	// so we don't have to optimise this any further.
	// https://godbolt.org/z/xfzvc1no1

	// Using Kernighan's algorithm.

	T count = 0;

	while (n)
	{
		count++;
		n &= n - 1;
	}

	return count;
}

/**
 * Returns the number of leading zeroes of a number.
 * This equals to the number of consecutive zeroes at the left side of the
 * binary representation of the number.
 * E.g. the number of leading zeroes of the 8-bit integer 37 is 2,
 * because 37 = 0b00100101, which has two leading zeroes.
 *
 * TODO: Research if this function can be optimised further for 8 and 16 bit
 * integers. Currently, using this function with 8 and 16 bit integers
 * results in an many extra instructions.
 */
template <typename T>
constexpr inline T
clz(T n)
{
	// Using builtins for 32 and 64 bit integers.
	// 8 and 16 bit integers will use the 32 bit builtin, but the number
	// will be shifted to the left.

	if constexpr (sizeof(T) == 8)
	{
		return __builtin_clzll(n);
	}

	if constexpr (sizeof(T) == 4)
	{
		return __builtin_clz(n);
	}

	if constexpr (sizeof(T) == 2)
	{
		return min(16, __builtin_clz(n << 16));
	}

	if constexpr (sizeof(T) == 1)
	{
		return min(8, __builtin_clz(n << 24));
	}

	throw "Type is not a valid integer type.";
}
/**
 * Returns the number of trailing zeroes of a number.
 * This equals to the number of consecutive zeroes at the left side of the
 * binary representation of the number.
 * E.g. the number of trailing zeroes of the 8-bit integer 60 is 2,
 * because 60 = 0b00111100, which has two trailing zeroes.
 *
 * TODO: Research if this function can be optimised further for 8 and 16 bit
 * integers. Currently, using this function with 8 and 16 bit integers
 * results in an many extra instructions.
 */
template <typename T>
constexpr inline T
ctz(T n)
{
	// Using builtins for 32 and 64 bit integers.
	// 8 and 16 bit integers will use the 32 bit builtin, but the number
	// will be shifted to the left.

	if constexpr (sizeof(T) == 8)
	{
		return __builtin_ctzll(n);
	}

	if constexpr (sizeof(T) == 4)
	{
		return __builtin_ctz(n);
	}

	if constexpr (sizeof(T) == 2)
	{
		return min(16, __builtin_ctz(n));
	}

	if constexpr (sizeof(T) == 1)
	{
		return min(8, __builtin_ctz(n));
	}

	throw "Type is not a valid integer type.";
}

namespace detail
{
/**
 * Helper function for the gcd function.
 */
template <typename T>
constexpr inline T
gcd_impl(T a, T b)
{
	// 0 is divisible by everything, so we return the other number.
	// The gcd of 0 and 0 is usually defined as 0, so that's okay.

	if (a == 0)
	{
		return b;
	}

	if (b == 0)
	{
		return a;
	}

	// We will use Stein's algorithm to find the gcd.
	// Stein's algorithm is based on Euclidean algorithm, but it makes
	// use of bitwise operations to optimise the divisions used in the
	// standard Euclidean algorithm.

	// To reduce the number of operations we have to perform in the main
	// loop, we will normalise the numbers by shifting them to the right
	// as many spots as there are trailing zeroes.
	// The number with the smallest number of trailing zeroes will be the
	// greatest power of two that divides both numbers.
	// At the end of the loop, we will have to multiply the numbers by this
	// power. We optimise this using bit shifting instead of multiplication.

	// e.g.
	// a = 128, b = 60
	// => a = 0b10000000, b = 0b111100
	// normalised a = 0b1, b = 0b1111
	// The number with the lowest number of trailing zeroes is 0b111100,
	// which has two trailing zeroes.
	// Therefore, the greatest power of two that divides both numbers is
	// 0b100, which is 4.
	// After the main loop, we conclude that the greatest common divisor is
	// 1, and we multiply this by the greatest power of two (which is 4)
	// to get the real gcd. We conclude that the gcd is 4.

	// Normalise the numbers.

	T ctz_a = ctz(a);
	a >>= ctz_a;

	T ctz_b = ctz(b);
	b >>= ctz_b;

	// Find the greatest power of two that divides both numbers.

	T greatest_power_of_two_divisor = min(ctz_a, ctz_b);

	// In the main loop, we will continuously subtract the smallest number
	// from the other number. Eventually we will reach a state where the
	// smallest number is 0, and the other number is the gcd.

	while (true)
	{
		// Ensure that `a` holds the smallest number.

		if (a > b)
		{
			swap(a, b);
		}

		// Subtract the smallest number from the other number.

		b -= a;

		// If the other number is 0, we have found the gcd.

		if (b == 0)
		{
			return a << greatest_power_of_two_divisor;
		}

		// If possible, we will normalise the bigger number.
		// This reduces the number of further operations we have to
		// perform in the remainder of main loop.
		// The smallest number is already normalised, because it was
		// either normalised before the loop, or it was the bigger
		// number in a previous iteration.

		b >>= ctz(b);
	}
}
}; // namespace slaw::detail

/**
 * Returns the greatest common divisor of two numbers.
 */
template <typename T>
constexpr inline T
gcd(T a, T b)
{
	if constexpr (is_unsigned<T>())
	{
		// If the type is unsigned,
		// we can simply call the gcd implementation.

		return detail::gcd_impl(a, b);
	}

	if constexpr (is_signed<T>())
	{
		// If the type is unsigned,
		// we have to ensure that the numbers are positive.

		if (a < 0)
		{
			a = -a;
		}

		if (b < 0)
		{
			b = -b;
		}

		return detail::gcd_impl(a, b);
	}
}
}; // namespace slaw

#endif