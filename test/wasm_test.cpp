#include "../slaw.hpp"

EXPORT("simd_test")
f32x4
simd_test(f32x4 v)
{
	return slaw::simd::floor(v);
}