#ifndef SLAW_MEM_DEBUG_H
#define SLAW_MEM_DEBUG_H

// If the memory allocator is disabled, then we cannot debug it.
#ifndef NO_MEMORY_ALLOCATOR

#include "mem.hpp"

/**
 * This namespace contains debugging utilities for the memory allocator.
 * Useful to debug memory issues in a WebAssembly environment.
 */
namespace slaw::mem_debug
{
/**
 * Returns the current heap size.
 */
usize
heap_size()
{
	return mem::heap_end - &mem::__heap_base;
}

/**
 * Returns the number of actual allocated bytes.
 */
usize
allocated_heap_size()
{
	if (heap_size() == 0)
	{
		return 0;
	}

	// Get the first block on the heap.

	mem::HeapBlockHeader *block =
		(mem::HeapBlockHeader *) &mem::__heap_base;

	// Iterate over the heap and count the number of allocated bytes.

	usize allocated_size = 0;

	while ((u8 *) block < mem::heap_end)
	{
		if (!block->is_free())
		{
			// The block is allocated, add its size to the total.

			allocated_size += block->size;
		}

		// Get the next block.

		block = block->next_block();
	}

	return allocated_size;
}

/**
 * Returns the number of free bytes on the heap.
 * This is the difference between the heap size and the allocated heap size.
 */
usize
free_heap_size()
{
	return heap_size() - allocated_heap_size();
}

/**
 * Returns a pointer to the start of the heap.
 */
u8 *
heap_start()
{
	return &mem::__heap_base;
}
};

#endif // NO_MEMORY_ALLOCATOR
#endif // SLAW_MEM_DEBUG_H