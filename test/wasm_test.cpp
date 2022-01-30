#include "../slaw.hpp"

EXPORT("from_float")
void
from_float(f64 n, usize p = 6)
{
	slaw::print(slaw::String::from_float(n, p));
}

EXPORT("from_int")
void
from_int(i32 n)
{
	slaw::print(slaw::String::from_int(n));
}