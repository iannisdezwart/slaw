#ifndef SLAW_STRING_H
#define SLAW_STRING_H

#include "types.hpp"
#include "vector.hpp"
#include "util.hpp"

namespace slaw
{
/**
 * String representation. Stores a buffer of characters and its length.
 * Allows constant-time access to the characters in any order.
 */
struct String : public Vector<char>
{
	/**
	 * Constructs an empty string.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String(usize initial_size = min_capacity)
		: Vector<char>(initial_size) {}

	/**
	 * Constructs a string by taking a copy of an existing string.
	 * The new string will have the same size and capacity as the source.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	String(const String &source)
		: Vector<char>(source) {}

	/**
	 * Constructs a string by moving an existing string.
	 * The new string will have the same size and capacity as the source.
	 * The source string will be emptied.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String(String &&source)
		: Vector<char>(move(source)) {}

	/**
	 * Copies a new string into this string.
	 * The new string will have the same size and capacity as the source.
	 * The original string will be overwritten.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	String &
	operator=(const String &source)
	{
		Vector<char>::operator=(source);
		return *this;
	}

	/**
	 * Moves a new string into this string.
	 * The new string will have the same size and capacity as the source.
	 * The source string will be emptied.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String &
	operator=(String &&source)
	{
		Vector<char>::operator=(move(source));
		return *this;
	}

	/**
	 * Constructs a string from a character array.
	 * The new string will have the same size and capacity as the source.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	String(const char (&source)[N])
		: Vector<char>(N)
	{
		for (usize i = 0; i < N - 1; i++)
		{
			data[i] = source[i];
		}
	}

	/**
	 * Copies a character array into this string.
	 * The new string will have the same size and capacity as the source.
	 * The original string will be overwritten.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	String &
	operator=(const char (&source)[N])
	{
		// Clear the string and reserve enough space for the new string.

		clear();
		realloc(N - 1);

		// Copy the characters into the string.

		for (usize i = 0; i < N - 1; i++)
		{
			data[i] = source[i];
		}

		return *this;
	}

	/**
	 * Appends a character to this string.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	void
	operator+=(char c)
	{
		push_back(c);
	}

	/**
	 * Appends a string to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	void
	operator+=(const String &s)
	{
		attach(s);
	}

	/**
	 * Appends a character array to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	void
	operator+=(const char (&s)[N])
	{
		// Reserve enough space for the new characters.

		realloc(size + N - 1);

		// Copy the characters into the string.

		for (usize i = 0; i < N - 1; i++)
		{
			data[size + i] = s[i];
		}
	}

	/**
	 * Creates a new string by appending a character to this string.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String
	operator+(char c) const
	{
		String s(*this);
		s += c;
		return s;
	}

	/**
	 * Creates a new string by appending a string to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	String
	operator+(const String &s) const
	{
		String t(*this);
		t += s;
		return t;
	}

	/**
	 * Creates a new string by appending a character array to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	String
	operator+(const char (&s)[N]) const
	{
		String t(*this);
		t += s;
		return t;
	}

	/**
	 * Checks if two strings are equal.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	operator==(const String &s) const
	{
		if (size != s.size)
		{
			return false;
		}

		for (usize i = 0; i < size; i++)
		{
			if (data[i] != s.data[i])
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Checks if two strings are not equal.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	operator!=(const String &s) const
	{
		return !operator==(s);
	}

	/**
	 * Repeats the contents of this string a given number of times.
	 * The string will be modified in-place.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	void
	repeat_in_place(usize n)
	{
		// Reserve enough space for the new characters.

		realloc(size * n);

		// Copy the characters into the string.

		for (usize i = 0; i < n; i++)
		{
			for (usize j = 0; j < size; j++)
			{
				data[i * size + j] = data[j];
			}
		}
	}

	/**
	 * Creates a new string by repeating the contents of this string
	 * a given number of times.
	 * The original string will not be modified.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	String
	repeat(usize n) const
	{
		// Create a new string of enough size.

		String s(size * n);

		// Copy the characters into the string.

		for (usize i = 0; i < n; i++)
		{
			for (usize j = 0; j < size; j++)
			{
				s.data[i * size + j] = data[j];
			}
		}

		return s;
	}

	/**
	 * Checks if this string starts with a given string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	starts_with(const String &s) const
	{
		if (size < s.size)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < s.size; i++)
		{
			if (data[i] != s.data[i])
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Checks if this string starts with a given character array.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	bool
	starts_with(const char (&s)[N]) const
	{
		if (size < N - 1)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < N - 1; i++)
		{
			if (data[i] != s[i])
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Checks if this string ends with a given string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	ends_with(const String &s) const
	{
		if (size < s.size)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < s.size; i++)
		{
			if (data[size - s.size + i] != s.data[i])
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Checks if this string ends with a given character array.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	bool
	ends_with(const char (&s)[N]) const
	{
		if (size < N - 1)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < N - 1; i++)
		{
			if (data[size - N + i] != s[i])
			{
				return false;
			}
		}

		return true;
	}

	// Prevent C++ inherited class name hiding.

	using Vector::contains;

	/**
	 * Checks if this string contains a given character array.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	bool
	contains(const char (&s)[N]) const
	{
		// Check if the character array is empty.

		if (N == 1)
		{
			return false;
		}

		// Check if the character array is longer than the string.

		if (N - 1 > size)
		{
			return false;
		}

		// Check if the character array is contained in the string.

		for (usize i = 0; i < size - N + 1; i++)
		{
			bool found = true;

			for (usize j = 0; j < N - 1; j++)
			{
				if (data[i + j] != s[j])
				{
					found = false;
					break;
				}
			}

			if (found)
			{
				return true;
			}
		}

		return false;
	}
};
}; // namespace slaw

#endif