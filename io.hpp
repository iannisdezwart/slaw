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
extern "C" void
eval_helper(const char *code, usize length);
};

namespace slaw
{
/**
 * Evaluates a string of JavaScript code.
 */
inline void
eval(const String &str)
{
	detail::eval_helper(str.data, str.size);
}

/**
 * Function that prints a string to the JavaScipt console.
 */
inline void
print(const String &str)
{
	eval("console.log('" + str + "')");
}
};

#endif