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

EXPORT("heap_size")
usize
get_heap_size()
{
	return slaw::mem_debug::heap_size();
}

EXPORT("allocated_heap_size")
usize
get_allocated_heap_size()
{
	return slaw::mem_debug::allocated_heap_size();
}

EXPORT("free_heap_size")
usize
get_free_heap_size()
{
	return slaw::mem_debug::free_heap_size();
}

EXPORT("heap_start")
u8 *
get_heap_start()
{
	return slaw::mem_debug::heap_start();
}