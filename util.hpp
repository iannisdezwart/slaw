#ifndef SLAW_UTIL_H
#define SLAW_UTIL_H

namespace slaw
{
/**
 * If this type is a reference, this removes the reference and returns a
 * compile-time struct with a field `type` that is the type of the reference.
 * This is useful for move constructors that take a reference.
 */
template <typename T>
struct remove_reference
{
	typedef T type;
};

/**
 * If this type is a reference, this removes the reference and returns a
 * compile-time struct with a field `type` that is the type of the reference.
 * This is useful for move constructors that take a reference.
 */
template <typename T>
struct remove_reference<T&>
{
	typedef T type;
};

/**
 * If this type is a reference, this removes the reference and returns a
 * compile-time struct with a field `type` that is the type of the reference.
 * This is useful for move constructors that take a reference.
 */
template <typename T>
struct remove_reference<T&&>
{
	typedef T type;
};

/**
 * Casts a value to an xvalue.
 * This allows the compiler to understand move semantics.
 */
template <typename T>
typename remove_reference<T>::type&&
move(T &&t)
{
	return static_cast<typename remove_reference<T>::type&&>(t);
}

/**
 * Returns the minimum of two values.
 */
template <typename T>
constexpr inline T
min(T a, T b)
{
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
 * Swaps two values.
 */
template <typename T>
constexpr inline void
swap(T &a, T &b)
{
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}
};

#endif