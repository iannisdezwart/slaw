#include "../slaw.hpp"

EXPORT("simd_test")
i32
simd_test()
{
	i32x4 a = { 1, 6, 3, 8 };
	i32x4 b = { 5, 2, 7, 4 };
	i32x4 c = slaw::simd::min(a, b);
	return slaw::simd::sum(c);
}