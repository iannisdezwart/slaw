# SLAW

Standard Library for Assemblifying the Web.


## Why

WebAssembly is cool, but the way it is used right now is horrible.
WebAssembly was never meant to be used to port native applications to the web
without any code changes.

Currently, there doesn't exist a proper C++ standard library used to build
WebAssembly applications. You can compile C++ code to WebAssembly, but you
cannot use the C++ standard library. Tools like
[emscripten](https://emscripten.org/) try to emulate libc and the C++ standard
library, but this results in extremely bloated executable sizes.
It is a quick way to port your existing C++ code to WebAssembly, but it doesn't
always work as expected and it is very slow.

This library tries to solve all problems and provide a standard library that
can be used to build WebAssembly applications in C++ that don't suck.
You can expect executable sizes of a couple of kilobytes and near-native
performance. This is what WebAssembly should be for.


## How

SLAW provides a memory allocator that is designed for WebAssembly specifically.
This memory allocator is used in many of the SLAW data structures.
This memory allocator is completely integrated with C++ overloads, and can
be used by simply using the `new` and `delete` operators.

SLAW also provides some utility functions you will find in a normal C++
standard library, like `std::move()` (as `slaw::move()`), `std::swap()`
(as `slaw::swap()`) and `std::min()` (as `slaw::min()`).


## Example Program Using SLAW

```cpp
#include "slaw.hpp"

slaw::Vector<i32>
add(slaw::Vector<i32> a, slaw::Vector<i32> b)
{
	slaw::Vector<i32> vec(a.size);

	for (usize i = 0; i < a.size; i++)
	{
		vec[i] = a[i] + b[i];
	}

	return vec;
}
```