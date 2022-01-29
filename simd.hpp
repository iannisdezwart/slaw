#ifndef SLAW_SIMD_H
#define SLAW_SIMD_H

#include "types.hpp"

// Below are the SIMD types that are supported by WebAssembly.

typedef i8 i8x16 __attribute__((__vector_size__(16)));
typedef i16 u16x8 __attribute__((__vector_size__(16)));
typedef i32 i32x4 __attribute__((__vector_size__(16)));
typedef i64 i64x2 __attribute__((__vector_size__(16)));

typedef f32 f32x4 __attribute__((__vector_size__(16)));
typedef f64 f64x2 __attribute__((__vector_size__(16)));

#endif