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
 * Swaps two values by copy.
 */
template <typename T>
constexpr inline void
swap(T &a, T &b)
{
	T tmp = a;
	a = b;
	b = tmp;
}

/**
 * Swaps two values using move semantics.
 */
template <typename T>
constexpr inline void
swap(T &&a, T &&b)
{
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}
};

#endif