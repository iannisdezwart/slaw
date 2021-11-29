#ifndef SLAW_MEM_H
#define SLAW_MEM_H

#include "types.hpp"

/**
 * This namespace contains functions for low-level memory management.
 * It contains the `slaw::mem::alloc()` and `slaw::mem::free()` functions
 * as well as some internal functions used by these functions.
 *
 * This namespace contains a couple of global variables that are used
 * by some of the functions in this namespace.
 * These variables are:
 * - `slaw::mem::heap_end`: The end of the heap segment.
 *   Before any allocations are made, this variable can be overwritten for
 *   testing purposes.
 * - `slaw::mem::first_free_block`: A pointer to the first free block in the
 *   heap. Before any allocations are made, this variable is set to null.
 * - `slaw::mem::last_block`: A pointer to the very last block of the heap.
 */
namespace slaw::mem
{
extern "C"
{
// The WebAssembly compiler provides us with a global label that we can use
// to refer to the base of the heap.
// We can take the address of this global label and use it as a pointer to
// the start of the heap memory section.
// The heap grows upwards and we can grow it as much as we want.
extern u8 __heap_base;
};


/**
 * Structure of a memory block header.
 *
 * The header comes before the actual memory block.
 * It contains the size of the block, flags (currently only used to check
 * whether the block is free or not), and a pointer to the previous block.
 * We don't need a pointer to the next block because we can simply find tthishe
 * by adding the size of the current block to the current block's header.
 *
 * If the block is free, the header can be cast to a `FreeHeapBlockHeader`.
 * This header also contains a pointer to the next free block and
 * the previous free block.
 */
struct HeapBlockHeader
{
	/**
	 * Used to indicate whether the block is free or not.
	 */
	static const constexpr u8 free_bit = 1 << 0;

	// Flags. Currently only used to check whether the block is free or not.
	u8 flags;

	// The inner size of the block. This is the gross size of the block
	// (the actual number of bytes allocated) minus the size of the header.
	usize size;

	// Pointer to the previous block. Used to find the previous free block.
	HeapBlockHeader *prev_block;

	/**
	 * @brief Checks if the `alloc_free_bit` is set.
	 * @returns A boolean indicating whether this block is free.
	 */
	inline bool
	is_free()
	const
	{
		return flags & free_bit;
	}

	/**
	 * @brief Marks the block free by setting the `alloc_free_bit`.
	 */
	inline void
	set_free()
	{
		flags |= free_bit;
	}

	/**
	 * @brief Marks the block allocated by clearing the `alloc_free_bit`.
	 */
	inline void
	set_allocated()
	{
		flags &= ~free_bit;
	}

	/**
	 * @returns A pointer to the start of the inner memory block.
	 */
	inline u8 *
	get_start_ptr()
	const
	{
		return (u8 *) this + sizeof(HeapBlockHeader);
	}

	/**
	 * @returns A pointer to one byte past the end of
	 * the inner memory block.
	 */
	inline u8 *
	get_end_ptr()
	const
	{
		return (u8 *) this + sizeof(HeapBlockHeader) + size;
	}

	/**
	 * @returns A pointer to the next block.
	 */
	inline HeapBlockHeader *
	next_block()
	const
	{
		return (HeapBlockHeader *) get_end_ptr();
	}
};

/**
 * Structure of a free memory block header.
 * Inherits from `HeapBlockHeader`.
 *
 * The free header comes before the actual memory block.
 * It contains everything of `HeapBlockHeader`, as well as a pointer
 * to the previous free block and a pointer to the next free block.
 * These pointers are useful, because they allow us to quickly go over
 * the free blocks in the heap. This allows us to find a block we can
 * use to allocate memory quicker.
 *
 * If this block is the first free block, the previous pointer is null.
 * If this block is the last free block, the next pointer is null.
 */
struct FreeHeapBlockHeader : public HeapBlockHeader
{
	// Pointer to the previous free block.
	FreeHeapBlockHeader *prev_free_block;

	// Pointer to the next free block.
	FreeHeapBlockHeader *next_free_block;

	/**
	 * @brief Removes this block from the free list.
	 * The previous and next free blocks are updated accordingly.
	 */
	inline void
	remove_from_free_block_list()
	{
		if (prev_free_block)
		{
			prev_free_block->next_free_block = next_free_block;
		}

		if (next_free_block)
		{
			next_free_block->prev_free_block = prev_free_block;
		}
	}
};

// Holds a pointer to the current end of the heap.
// This pointer can be increased if there is not enough space on the heap
// to allocate block of certain size.
u8 *heap_end = &__heap_base;

// Holds a pointer to the first free block.
// Is null if there is no free block.
FreeHeapBlockHeader *first_free_block = nullptr;

// Holds a pointer to the last block.
// Is null if there are no blocks.
HeapBlockHeader *last_block = nullptr;

/**
 * @brief Allocates memory at the end of the heap.
 * The heap_end pointer is increased by the size of the full block.
 * This function is useful when we want to allocate a block of memory
 * quickly. It is also used internally by the `slaw::mem::alloc` function
 * if there is no free block that would fit the requested size.
 *
 * @param size The inner size of the block to allocate.
 * @returns A pointer to the allocated block.
 */
void *
alloc_end(usize size)
{
	// Add a header to the end of the heap.

	HeapBlockHeader *header = (HeapBlockHeader *) heap_end;

	// Increase the `heap_end` pointer by the size of the full block.
	// The full block contains the header and the inner size.

	heap_end += sizeof(HeapBlockHeader) + size;

	// Set the size of the block.

	header->size = size;

	// Mark the block as allocated.

	header->set_allocated();

	// Store a pointer to the previous block and make this the last block.

	header->prev_block = last_block;
	last_block = header;

	// Return a pointer to the start of the inner block.

	return header->get_start_ptr();
}

/**
 * @brief Dynamically allocates memory on the heap.
 *
 * @param size The number of bytes to allocate.
 * @returns A pointer to the allocated block.
 * This pointer must be passed to `slaw::mem::free()` at some point,
 * or memory leaks will occur.
 */
void *
alloc(usize size)
{
	// If the provided size is less than the size of a free block header,
	// we will allocate a block that is as large as a free block header.
	// We do this because we want to ensure we can add a pointer to the
	// previous and next free block to the free block header.

	if (size < sizeof(FreeHeapBlockHeader))
	{
		size = sizeof(FreeHeapBlockHeader);
	}

	// If there is no free block, allocate at the end of the heap.

	if (first_free_block == nullptr)
	{
		return alloc_end(size);
	}

	// Find the first free block that is large enough.

	FreeHeapBlockHeader *free_block = first_free_block;

	while (free_block != nullptr && free_block->size < size)
	{
		free_block = free_block->next_free_block;
	}

	// If no free block is large enough, allocate at the end of the heap.

	if (free_block == nullptr)
	{
		return alloc_end(size);
	}

	// If the free block is large enough to also put a header,
	// we will split the free block into two parts:
	// The first part is the remaining free block.
	// The second part is the block we are allocating.
	// TODO: Consider changing the sizeof(HeapBlockHeader) to a constant
	// that can be modified.

	if (free_block->size - size >= sizeof(HeapBlockHeader))
	{
		// The block is large enough to also put the header.
		// We will split the free block into two parts.
		// Get a pointer to the start of the new header.

		HeapBlockHeader *new_block = (HeapBlockHeader *)
			((u8 *) free_block->get_end_ptr()
			- size - sizeof(HeapBlockHeader));

		// Set the size of the new header and mark it as allocated.
		// Also link it to the previous block.

		new_block->size = size;
		new_block->set_allocated();
		new_block->prev_block = free_block;

		// Update the size of the free block.

		free_block->size = free_block->size
			- sizeof(HeapBlockHeader) - size;

		// Make the next block point back to the new block.

		HeapBlockHeader *next_block = (HeapBlockHeader *)
			new_block->get_end_ptr();

		next_block->prev_block = new_block;

		// Return a pointer to the start of the new inner block.

		return new_block->get_start_ptr();
	}

	// The free block does not contain enough size to also put the header.
	// We will allocate the entire free block.
	// Remove the block from the list of free blocks.

	free_block->remove_from_free_block_list();

	// Mark the block as allocated.

	free_block->set_allocated();

	// Check if the free block is the first free block.
	// If it is, we need to update the first free block pointer.

	if (free_block == first_free_block)
	{
		first_free_block = free_block->next_free_block;
	}

	// We just return a pointer to the start of this block.
	// There is no need to update the size of the block.

	return free_block->get_start_ptr();
}

/**
 * @brief Merges two free blocks into one big free block.
 *
 * @param block1 The first free block.
 * @param block2 The second free block.
 */
void
merge_two_free_blocks(FreeHeapBlockHeader *block1, FreeHeapBlockHeader *block2)
{
	// Get a pointer to the next block.
	// We have to update its previous block pointer.

	HeapBlockHeader *next_block = (HeapBlockHeader *) block2->get_end_ptr();

	// Update the previous pointer of the next block.
	// The next block will point back to the first block.

	next_block->prev_block = block1;

	// Update the size of the first block.
	// We add the size of the second block and the size of the header.

	block1->size += block2->size + sizeof(HeapBlockHeader);

	// Remove the second block from the free list.

	block2->remove_from_free_block_list();
}

/**
 * @brief Merges three free blocks into one big free block.
 *
 * @param block1 The first free block.
 * @param block2 The second free block.
 * @param block3 The third free block.
 */
void
merge_three_free_blocks(FreeHeapBlockHeader *block1,
	FreeHeapBlockHeader *block2, FreeHeapBlockHeader *block3)
{
	// Get a pointer to the next block.
	// We have to update its previous block pointer.

	HeapBlockHeader *next_block = (HeapBlockHeader *) block3->get_end_ptr();

	// Update the previous pointer of the next block.
	// The next block will point back to the first block.

	next_block->prev_block = block1;

	// Update the size of the first block.
	// We add the size of the second and third block
	// and the size of their headers.

	block1->size += block2->size + block3->size
		+ sizeof(FreeHeapBlockHeader) * 2;

	// Remove the second and third block from the free list.

	block2->remove_from_free_block_list();
	block3->remove_from_free_block_list();
}

/**
 * @brief Checks if a free memory block can be merged with
 * its neigbouring blocks.
 * If this is possible, the blocks are merged.
 *
 * @param block The free block to check.
 */
void
maybe_merge_free_blocks(FreeHeapBlockHeader *block)
{
	bool prev_exists = block->prev_block != nullptr;
	bool next_exists = block->get_end_ptr() != heap_end;

	// If both the previous and next blocks are free,
	// merge them with the current block.

	if (prev_exists && block->prev_block->is_free()
		&& next_exists && block->next_block()->is_free())
	{
		merge_three_free_blocks(
			(FreeHeapBlockHeader *) block->prev_block, block,
			(FreeHeapBlockHeader *) block->next_block());
	}

	// If only the previous block is free, merge it with the current block.

	else if (prev_exists && block->prev_block->is_free())
	{
		merge_two_free_blocks(
			(FreeHeapBlockHeader *) block->prev_block, block);
	}

	// If only the next block is free, merge it with the current block.

	else if (next_exists && block->next_block()->is_free())
	{
		merge_two_free_blocks(
			block, (FreeHeapBlockHeader *) block->next_block());
	}

	// The neigbouring blocks are both allocated.
	// We cannot merge anything, so we return.
}

/**
 * @brief Frees memory on the heap.
 *
 * @param ptr The pointer to the block to free.
 * This must be a pointer returned by `slaw::mem::alloc()`.
 */
void
free(void *ptr)
{
	// Get a pointer to the header of the block we are freeing.

	FreeHeapBlockHeader *block = (FreeHeapBlockHeader *)
		((HeapBlockHeader *) ptr - 1);

	// If the block is already free, return.

	if (block->is_free())
	{
		return;
	}

	// Mark the block as free.

	block->set_free();

	// If there are no free blocks,
	// the block we are freeing will be the first free block.

	if (first_free_block == nullptr)
	{
		first_free_block = block;

		block->prev_free_block = nullptr;
		block->next_free_block = nullptr;

		return;
	}

	// If the block we are freeing is before the first free block,
	// we will insert the block before the first free block.

	if (block < first_free_block)
	{
		// Prepend this block to the old first free block.

		first_free_block->prev_free_block = block;
		block->next_free_block = first_free_block;
		block->prev_free_block = nullptr;

		// Set this block to be the first free block.

		first_free_block = block;

		// Check if we can merge anything.

		maybe_merge_free_blocks(block);
		return;
	}

	// If the block we are freeing is the last block,
	// we will decrement the pointer to the end of the heap.

	if (block->get_end_ptr() >= (u8 *) heap_end)
	{
		// If the previous block is free, we will discard both
		// blocks and shrink the heap by these two blocks.

		if (block->prev_block != nullptr && block->is_free())
		{
			FreeHeapBlockHeader *prev_block =
				(FreeHeapBlockHeader *) block->prev_block;

			prev_block->remove_from_free_block_list();
			heap_end = (u8 *) prev_block;
		}

		// Else, we just discard the block we are freeing and
		// shrink the heap by one block.

		else
		{
			heap_end = (u8 *) block;
		}

		return;
	}

	// The block we are freeing is somewhere in the middle of the heap.
	// It is guaranteed that there is a free block both before it
	// and after it. We will have to search the previous free block.

	HeapBlockHeader *search = block->prev_block;

	while (!search->is_free())
	{
		search = search->prev_block;
	}

	FreeHeapBlockHeader *prev_free_block = (FreeHeapBlockHeader *) search;

	// Attach this block to the previous free block and the next free block.

	block->prev_free_block = prev_free_block;
	block->next_free_block = prev_free_block->next_free_block;

	// We have to check if the next block exists, because it might not.
	// If it does exist, we set the previous pointer of the next block
	// to the new free block.

	if (prev_free_block->next_free_block)
	{
		prev_free_block->next_free_block->prev_free_block = block;
	}

	prev_free_block->next_free_block = block;

	// Check if we can merge anything.

	maybe_merge_free_blocks(block);
}
}; // namespace slaw::mem

/**
 * @brief Dynamically allocates memory on the heap.
 *
 * @param size The number of bytes to allocate.
 * @returns A pointer to the allocated block.
 * This pointer must be `delete`d at some point,
 * or memory leaks will occur.
 */
void *
operator new(usize size)
{
	return slaw::mem::alloc(size);
}

/**
 * @brief Dynamically allocates an array on the heap.
 *
 * @param size The number of elements to allocate.
 * @returns A pointer to the allocated array.
 * This pointer must be `delete[]`d at some point,
 * or memory leaks will occur.
 */
void *
operator new[](usize size)
{
	return slaw::mem::alloc(size);
}

/**
 * @brief Frees memory on the heap.
 *
 * @param ptr The pointer to the block to free.
 * This must be a pointer returned by the `new` operator.
 */
void
operator delete(void *ptr)
noexcept
{
	slaw::mem::free(ptr);
}

/**
 * @brief Frees an array on the heap.
 *
 * @param ptr The pointer to the array to free.
 * This must be a pointer returned by the `new[]` operator.
 */
void
operator delete[](void *ptr)
noexcept
{
	slaw::mem::free(ptr);
}

#endif