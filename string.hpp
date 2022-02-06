#ifndef SLAW_STRING_H
#define SLAW_STRING_H

#include "types.hpp"
#include "math.hpp"
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
	 * Constructs an empty string with a given initial capacity.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String(usize initial_capacity = min_capacity)
		: Vector<char>(initial_capacity) {}

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
		: Vector<char>(N - 1)
	{
		for (usize i = 0; i < N - 1; i++)
		{
			data[i] = source[i];
		}

		size = N - 1;
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

		size = N - 1;

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
		attach_back(s);
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

		// Update the size.

		size += N - 1;
	}

	/**
	 * Creates a new string by appending a character to this string.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	String
	operator+(char c)
	const
	{
		// Create a new string of the correct size.

		String out(size + 1);
		out.size = size + 1;

		// Copy the left hand side into the new string.

		for (usize i = 0; i < size; i++)
		{
			out[i] = data[i];
		}

		// Copy the extra character into the new string.

		out[size] = c;
		return out;
	}

	/**
	 * Creates a new string by appending a string to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	String
	operator+(const String &s)
	const
	{
		// Create a new string of the correct size.

		String out(size + s.size);
		out.size = size + s.size;

		// Copy the left hand side into the new string.

		for (usize i = 0; i < size; i++)
		{
			out[i] = data[i];
		}

		// Copy the right hand side into the new string.

		for (usize i = 0; i < s.size; i++)
		{
			out[size + i] = s[i];
		}

		return out;
	}

	/**
	 * Creates a new string by appending a character array to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	String
	operator+(const char (&s)[N])
	const
	{
		// Create a new string of the correct size.

		String out(size + N - 1);
		out.size = size + N - 1;

		// Copy the left hand side into the new string.

		for (usize i = 0; i < size; i++)
		{
			out[i] = data[i];
		}

		// Copy the right hand side into the new string.

		for (usize i = 0; i < N - 1; i++)
		{
			out[size + i] = s[i];
		}

		return out;
	}

	/**
	 * Creates a new string by prepending a character array to this string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	template <usize N>
	friend String
	operator+(const char (&s)[N], const String &str)
	{
		// Create a new string of the correct size.

		String out(N - 1 + str.size);
		out.size = N - 1 + str.size;

		// Copy the characters from the character array into the string.

		for (usize i = 0; i < N - 1; i++)
		{
			out[i] = s[i];
		}

		// Copy the characters from the string into the string.

		for (usize i = 0; i < str.size; i++)
		{
			out[N - 1 + i] = str[i];
		}

		return out;
	}

	/**
	 * Checks if two strings are equal.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	operator==(const String &s)
	const
	{
		// If the strings are not the same size, they cannot be equal.

		if (size != s.size)
		{
			return false;
		}

		// Compare the characters in the strings.

		for (usize i = 0; i < size; i++)
		{
			if (data[i] != s.data[i])
			{
				// We found characters that are not equal,
				// so the strings are not equal.

				return false;
			}
		}

		// All characters in the strings are equal,
		// so the strings are equal.

		return true;
	}

	/**
	 * Checks if two strings are not equal.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	operator!=(const String &s)
	const
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

		// Update the size.

		size *= n;
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
	repeat(usize n)
	const
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
	starts_with(const String &s)
	const
	{
		// If the size of the string is smaller than the size of the
		// substring, then the string cannot possibly start with the
		// substring.

		if (size < s.size)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < s.size; i++)
		{
			if (data[i] != s.data[i])
			{
				// We found characters that are not equal,
				// so the string cannot possibly start with the
				// substring.

				return false;
			}
		}

		// The string starts with the substring.

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
	starts_with(const char (&s)[N])
	const
	{
		// If the size of the string is smaller than the size of the
		// substring, then the string cannot possibly start with the
		// substring.

		if (size < N - 1)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < N - 1; i++)
		{
			if (data[i] != s[i])
			{
				// We found characters that are not equal,
				// so the string cannot possibly start with the
				// substring.

				return false;
			}
		}

		// The string starts with the substring.

		return true;
	}

	/**
	 * Checks if this string ends with a given string.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	ends_with(const String &s)
	const
	{
		// If the size of the string is smaller than the size of the
		// substring, then the string cannot possibly end with the
		// substring.

		if (size < s.size)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < s.size; i++)
		{
			if (data[size - s.size + i] != s.data[i])
			{
				// We found characters that are not equal,
				// so the string cannot possibly end with the
				// substring.

				return false;
			}
		}

		// The string ends with the substring.

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
	ends_with(const char (&s)[N])
	const
	{
		// If the size of the string is smaller than the size of the
		// substring, then the string cannot possibly end with the
		// substring.

		if (size < N - 1)
		{
			return false;
		}

		// Compare the characters.

		for (usize i = 0; i < N - 1; i++)
		{
			if (data[size - N + i] != s[i])
			{
				// We found characters that are not equal,
				// so the string cannot possibly end with the
				// substring.

				return false;
			}
		}

		// The string ends with the substring.

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
	contains(const char (&s)[N])
	const
	{
		// If the character array is longer than the string,
		// the string cannot possibly contain the character array.

		if (N - 1 > size)
		{
			return false;
		}

		// Check if the character array is contained in the string.

		for (usize i = 0; i < size - N + 1; i++)
		{
			bool found = true;

			// For each index in the string, we check if the
			// characters from this index are equal to the
			// characters from the character array.

			for (usize j = 0; j < N - 1; j++)
			{
				if (data[i + j] != s[j])
				{
					// We found characters that are not
					// equal, so we will advance to the next
					// index.

					found = false;
					break;
				}
			}

			if (found)
			{
				// We found the character array in the string.

				return true;
			}
		}

		// The character array is not contained in the string.

		return false;
	}

	/**
	 * Pads the start of this string with a given character.
	 * The string is padded until it reaches a given size.
	 * If the string is already larger than the given size,
	 * nothing is done.
	 */
	void
	pad_start(char c, usize new_size)
	{
		// If the string is already larger than the given size,
		// then we do nothing.

		if (size >= new_size)
		{
			return;
		}

		// Pad the string at the start.

		usize pad_size = new_size - size;
		shift_right(pad_size);

		for (usize i = 0; i < pad_size; i++)
		{
			data[i] = c;
		}

		// We don't have to update the size of the string,
		// because `shift_right` already did that.
	}

	/**
	 * Pads the end of this string with a given character.
	 * The string is padded until it reaches a given size.
	 * If the string is already larger than the given size,
	 * nothing is done.
	 */
	void
	pad_end(char c, usize new_size)
	{
		// If the string is already larger than the given size,
		// then we do nothing.

		if (size >= new_size)
		{
			return;
		}

		// Pad the string at the end.

		usize pad_size = new_size - size;
		reserve(size + pad_size);

		for (usize i = 0; i < pad_size; i++)
		{
			data[size + i] = c;
		}

		// Update the size.

		size += pad_size;
	}

	/**
	 * Converts an integer to a string.
	 * TODO: Make this function more space efficient. Compiling a file
	 * with just this function exported with `-O3` yields a binary of
	 * about 3.9kB. `-Os` yields a binary of about 2.2kB.
	 */
	template <typename T>
	static String
	from_int(T i)
	{
		static_assert(is_integer<T>(), "Expected an integer type.");

		if (i == 0)
		{
			return "0";
		}

		// If the number is negative, make it positive and remember
		// that we have to add a '-' at the start of the string.

		bool sign = i < 0;

		if (sign)
		{
			i = -i;
		}

		// We reserve enough characters for the given integer type,
		// plus one for the sign.
		// The number of characters needed is equal to the logarithm
		// of the max value of the given integer type, plus one.

		const constexpr usize max_chars = log10i(max_value<T>()) + 2;
		String s(max_chars);

		// Go through each digit of the integer, from least to most
		// significant. We add each digit to the string.

		while (i > 0)
		{
			s.push_back('0' + (i % 10));
			i /= 10;
		}

		// Handle the sign.

		if (sign)
		{
			s.push_back('-');
			i = -i;
		}

		// The order of characters in the string is reversed,
		// so we reverse it again.

		s.reverse();
		return s;
	}

	/**
	 * Converts a floating point number to a string.
	 * The number is rounded to the given number of decimal places.
	 * TODO: Make this function more space efficient. Compiling a file
	 * with just this function exported with `-O3` yields a binary of
	 * about 4.4kB. `-Os` yields a binary of about 2.9kB.
	 */
	template <typename T>
	static String
	from_float(T f, usize precision = 6)
	{
		static_assert(is_float<T>(), "Expected a floating point type.");

		// Handle special cases.

		if (f == 0)
		{
			if (detail::stored_sign(f) == false)
			{
				return "0";
			}
			else
			{
				return "-0";
			}
		}

		if (is_nan(f))
		{
			return "NaN";
		}

		if (f == Infinity<T>())
		{
			return "Infinity";
		}

		if (f == -Infinity<T>())
		{
			return "-Infinity";
		}

		// If the number is negative, make it positive and remember
		// that we have to add a '-' at the start of the string.

		bool sign = f < 0;

		if (sign)
		{
			f = -f;
		}

		// We reserve enough characters for the given number, plus one
		// for the sign, plus one for the decimal point, plus the given
		// number of decimal places.

		usize left_digits = floor(max(0.0, log10(f)) + 1);
		usize max_chars = left_digits + precision + 2;
		String s(max_chars);

		// Handle the sign.

		if (sign)
		{
			s.push_back('-');
		}

		// Handle the rest of the number.

		T digit_place = pow(10.0, left_digits - 1);
		i32 digit;

		for (usize i = 0; i < left_digits + precision; i++)
		{
			// Get the digit.

			digit = floor(f / digit_place);

			// Remove the digit from the number.

			f -= digit * digit_place;
			digit_place /= 10;

			// If this is the last digit, check if we have to round.

			if (i == left_digits + precision - 1)
			{
				i32 next_digit = floor(f / digit_place);

				if (next_digit >= 5)
				{
					digit++;
				}
			}

			// If we have reached the decimal point,
			// we will add the decimal point to the string.

			if (i == left_digits)
			{
				s.push_back('.');
			}

			// Add the digit to the string.

			s.push_back('0' + digit);
		}

		return s;
	}
};
}; // namespace slaw

#endif