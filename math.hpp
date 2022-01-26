#ifndef SLAW_MATH_H
#define SLAW_MATH_H

#include "types.hpp"

namespace slaw
{
// Below are some common mathematical constants.

// Pi (π): the ratio of a circle's circumference to its diameter.
constexpr const f64 PI = 3.14159265358979323846;

// Half of PI: π / 2.
constexpr const f64 HALF_PI = PI / 2.0;

// Double of PI: π * 2.
constexpr const f64 TWO_PI = PI * 2.0;

// Euler's number (e): the base of the natural logarithm.
constexpr const f64 E = 2.71828182845904523536;

// ln(2): the natural logarithm of 2.
constexpr const f64 LN_2 = 0.69314718055994530941;

// ln(10): the natural logarithm of 10.
constexpr const f64 LN_10 = 2.30258509299404568402;

// log_2(e): the base-2 logarithm of e.
constexpr const f64 LOG_2_E = 1.44269504088896340736;

// log_2(10): the base-2 logarithm of 10.
constexpr const f64 LOG_2_10 = 3.32192809488736234787;

// log_10(e): the base-10 logarithm of e.
constexpr const f64 LOG_10_E = 0.43429448190325182765;

// log_10(2): the base-10 logarithm of 2.
constexpr const f64 LOG_10_2 = 0.30102999566398119521;

// √2: the square root of 2.
constexpr const f64 SQRT_2 = 1.41421356237309504880;

// 1 / √2: the inverse square root of 2.
// Equivalent to √0.5.
constexpr const f64 INV_SQRT_2 = 0.70710678118654752440;

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
	if constexpr (is_integer<T>() && is_unsigned<T>())
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
 * Rounds the given number down to the nearest integer.
 */
constexpr i64
floor(f32 x)
{
	return static_cast<i64>(x);
}

/**
 * Rounds the given number up to the nearest integer.
 */
constexpr i64
ceil(f32 x)
{
	return static_cast<i64>(x + 1);
}

/**
 * Rounds the given number to the nearest integer.
 * .50000... and above is rounded up, .49999... and below is rounded down.
 */
constexpr i64
round(f32 x)
{
	return static_cast<i64>(x + 0.5f);
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
	// Using builtins for 16, 32 and 64 bit integers.
	// 8 bit integers have to be implemented manually, but are compiled
	// into the same WASM code as the 16 bit built-in.

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
		return __builtin_clzs(n);
	}

	if constexpr (sizeof(T) == 1)
	{
		return (__builtin_clz(n & 0xFF) - 24) & 0xFF;
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
	// Using builtins for 16, 32 and 64 bit integers.
	// 8 bit integers will use the 32 bit builtin, but we will ensure
	// we get the correct result by ORing the input with 256.

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
		return __builtin_ctzs(n);
	}

	if constexpr (sizeof(T) == 1)
	{
		// We OR the number with 256, so the 8th bit is set to 1.
		// This allows us to get the correct result of 8, when the
		// input is 0.
		// TODO: for some reason, this results in an extra `AND n, 255`
		// instruction, which is not needed.

		return __builtin_ctz((i32) n | 0x100);
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

/**
 * Returns the square root of a number.
 */
constexpr f64
sqrt(f64 n)
{
	// If we evaluate this function at compile time,
	// we cannot use the builtin sqrt function.

	if (__builtin_is_constant_evaluated())
	{
		// We use Newton's method to find the square root.
		// Newton's method is an iterative method to find the root of a
		// given function. The formula for Newton's method is:
		// x_{n+1} = x_n - f(x_n) / f'(x_n)
		//
		// Since x = √n, which can be rewritten to x^2 - n = 0,
		// we can find the square root of n by finding the root of
		// the equation x^2 - n = 0. Note that the derivative of this
		// equation is 2x, which we will need in the formula for
		// Newton's method.
		//
		// We will start with an initial guess of x = n / 2.

		f64 x = n / 2;
		f64 delta = 1;

		// Now we will do a couple of iterations.
		// Note that we take f(x) = x^2 - n, and f'(x) = 2x.
		// We will stop when the difference between the current guess
		// and the previous guess is less than the smallest number
		// that can be represented by an f64.

		while (abs(delta) > Epsilon64)
		{
			f64 x_old = x;
			x = x - (x * x - n) / (2 * x);
			delta = x_old - x;
		}

		// The value of x will be the square root of n.

		return x;
	}

	// If we are not interpreting this function at compile time,
	// we will tell the compiler to use the f64.sqrt WASM instruction.
	// This function is not available at compile time, so that's why
	// we have all of this code above.

	return __builtin_sqrt(n);
}

namespace detail
{
/**
 * Returns the base-2 logarithm of a floating point number.
 */
template <typename T>
constexpr T
log2_impl(T value)
{
	// To compute the logarithm efficiently, we will use the fact that
	// log(A * B) = log(A) + log(B) and a Taylor series of the logarithm
	// centred around 1.
	//
	// We will split the input into two parts: one with just the
	// mantissa (A) and one with just the exponent (B).
	//
	// Since the mantissa is between 1 and 2, it is very close to the
	// centre of the Taylor series. So we can compute the logarithm of A
	// with great accuracy with only a few terms.
	//
	// log(B) is very easy to compute, because it is equal to the value
	// of the exponent.

	T a = detail::clear_exponent(value);
	i32 log_b = detail::stored_exponent(value);

	// We will use a Taylor series to compute the logarithm of A:
	// log2(x) = 2/ln(2) * [ (x-1)/(x+1) + (x-1)/(x+1)^3/3 + ... ]
	// The series converges very quickly around 1.

	// The series is centred at 1, but it will lose precision when we
	// approach 2, so we will shift the range from [1, 2) to [.75, 1.5).
	// We can easily do this by using log(x) = log(4/3) + log(.75x).
	// We will multiply our input by .75 and add log(4/3) to the result.

	a *= .75;

	// This is our initial value for log(A).

	T log_a = 0.14384103622589046371; // ln(4/3) / 2

	// We will now do a few iterations of the Taylor series to make the
	// value we have for log(A) more precise.

	T term = (a - 1) / (a + 1);

	for (u32 i = 0; i < 10; i++)
	{
		log_a += term / (2 * i + 1);
		term *= (a - 1) / (a + 1) * (a - 1) / (a + 1);
	}

	// Change base from ln to log2.

	log_a *= 2 / LN_2;

	// Add the exponent. We are now done.

	return log_a + log_b;
}
}; // namespace slaw::detail

/**
 * Returns the base-2 logarithm of a floating point number.
 */
constexpr f64
log2(f64 value)
{
	return detail::log2_impl(value);
}

/**
 * Returns the base-2 logarithm of a floating point number.
 */
constexpr f32
log2(f32 value)
{
	return detail::log2_impl(value);
}

/**
 * Returns the base-2 logarithm of a floating point number.
 */
template <typename T>
constexpr f64
log2(T value)
{
	return detail::log2_impl((f64) value);
}

/**
 * Returns the natural logarithm of a floating point number.
 */
template <typename T>
constexpr auto
ln(T value)
{
	return log2(value) * LN_2;
}

/**
 * Returns the base-10 logarithm of a floating point number.
 */
template <typename T>
constexpr auto
log10(T value)
{
	return log2(value) * LOG_10_2;
}

namespace detail
{
/**
 * Normalises an angle to the range [ 0, 2π ].
 */
constexpr inline f64
normalise_angle_around_pi(f64 angle)
{
	return angle - floor(angle / TWO_PI) * TWO_PI;
}

/**
 * Normalises an angle to the range [ -π, π ].
 */
constexpr inline f64
normalise_angle_around_zero(f64 angle)
{
	angle = normalise_angle_around_pi(angle);

	if (angle > PI)
	{
		angle -= TWO_PI;
	}

	return angle;
}
};

/**
 * Computes the sine of a given angle.
 */
f64
sin(f64 angle)
{
	// Use the Taylor series to compute the sine of an angle.
	// The series is: sin(x) = x - x^3 / 3! + x^5 / 5! - x^7 / 7! + ...
	// The series converges very quickly, so a couple of terms are enough.
	// TODO: research how many terms we need and research more efficient
	// ways of doing this.

	// Normalise the angle.

	angle = detail::normalise_angle_around_zero(angle);

	// Compute the sine.

	f64 first_power   = angle;
	f64 third_power   = first_power * angle * angle;
	f64 fifth_power   = third_power * angle * angle;
	f64 seventh_power = fifth_power * angle * angle;

	return first_power
		- third_power   / 6.0
		+ fifth_power   / 120.0
		- seventh_power / 5040.0;
}
}; // namespace slaw

#endif