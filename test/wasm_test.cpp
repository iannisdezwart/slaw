#include "../slaw.hpp"

slaw::Vector<u32>
create_arr(usize size)
{
	slaw::Vector<u32> vec;

	for (u32 i = 0; i < size; i++)
	{
		vec.push_back(i);
	}

	return vec;
}

u32
sum(slaw::Vector<u32> vec)
{
	u32 sum = 0;

	for (u32 v : vec)
	{
		sum += v;
	}

	return sum;
}

EXPORT("sum_0_to_n")
u32
sum_0_to_n(usize n)
{
	return sum(create_arr(n));
}

EXPORT("mem_at")
u8
mem_at(u8 *n)
{
	return *n;
}

EXPORT("allocate")
u8 *
allocate(usize size)
{
	u8 *mem = new u8[size];

	for (usize i = 0; i < size; i++)
	{
		mem[i] = i;
	}

	return mem;
}

EXPORT("get_heap_end")
u8 *
get_heap_end()
{
	return slaw::mem::heap_end;
}