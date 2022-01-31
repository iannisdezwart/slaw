#ifndef SLAW_IO_H
#define SLAW_IO_H

#include "types.hpp"
#include "export.hpp"
#include "string.hpp"

namespace slaw::detail
{
/**
 * JavaScript function that evaluates a string of JavaScript code.
 */
IMPORT("eval")
void
eval(const char *code, usize length);

/**
 * JavaScript function that prints a string.
 */
IMPORT("print_str")
void
print_str(const char *str, usize length);

/**
 * Wrapper around `print_str` that prints a character array without having
 * to specify the length.
 */
template <usize N>
inline void
print(const char (&str)[N])
{
	print_str(str, N - 1);
}

/**
 * JavaScript function that prints a number.
 */
IMPORT("print")
void
print(double number);
};

namespace slaw
{
/**
 * Evaluates a string of JavaScript code.
 */
inline void
eval(const String &str)
{
	detail::eval(str.data, str.size);
}

/**
 * Function that prints a string to the JavaScipt console.
 */
inline void
print(const String &str)
{
	detail::print_str(str.data, str.size);
}
};

#endif