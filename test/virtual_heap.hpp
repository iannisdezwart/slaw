#include "../types.hpp"
#include "../mem.hpp"

u8 *
create_heap(usize size)
{
	u8 *heap = new u8[size];
	slaw::mem::heap_end = heap;

	return heap;
}