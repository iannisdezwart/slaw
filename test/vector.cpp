#include <iostream>
#include "../types.hpp"
#include "../vector.hpp"

u8 __heap_base; // We don't touch this.

void
debug()
{
	printf("===========================================\n");
	printf("first_free_block = %p\n", slaw::mem::first_free_block);
	printf("heap_end         = %p\n", slaw::mem::heap_end);
	printf("\nBlocks:\n\n");

	auto block = (slaw::mem::HeapBlockHeader *) &__heap_base;

	while ((u8 *) block < slaw::mem::heap_end)
	{
		std::move(block);
		if (block->is_free())
		{
			printf("[free]\n");
		}
		else
		{
			printf("[used]\n");
		}

		printf("block = %p\n", block);
		printf("size  = %ld\n", block->size);
		printf("prev  = %p\n", block->prev_block);
		printf("start = %p\n", block->get_start_ptr());
		printf("end   = %p\n", block->get_end_ptr());
		printf("off   = %lu\n", (u8 *) block - &__heap_base);

		if (block->is_free())
		{
			auto *free_block = (slaw::mem::FreeHeapBlockHeader *)
				block;
			printf("next_free = %p\n", free_block->next_free_block);
			printf("prev_free = %p\n", free_block->prev_free_block);
		}

		printf("\n");

		block = (slaw::mem::HeapBlockHeader *) ((char *) block + sizeof(slaw::mem::HeapBlockHeader) + block->size);
	}

	printf("===========================================\n\n\n");
}

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
	void *ptr = slaw::mem::alloc(size);
	printf("operator new called with size = %lu, ptr = %p\n", size, ptr);
	debug();
	return ptr;
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
	void *ptr = slaw::mem::alloc(size);
	printf("operator new[] called with size = %lu, ptr = %p\n", size, ptr);
	debug();
	return ptr;
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
	printf("operator delete called on %p\n", ptr);
	debug();
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
	printf("operator delete[] called on %p\n", ptr);
	debug();
}

bool
is_prime(int i)
{
	if (i < 2)
	{
		return false;
	}

	for (int j = 2; j < i; j++)
	{
		if (i % j == 0)
		{
			return false;
		}
	}

	return true;
}

void
create_prime_array()
{
	slaw::Vector<int> v;

	for (int i = 0; i < 1000; i++)
	{
		v.push_back(i);

		if (!is_prime(i))
		{
			v.pop_back();
		}
	}

	for (int i : v)
	{
		printf("%d\n", i);
	}
}

int
main()
{
	printf("__heap_base = %p\n", &__heap_base);
	debug();

	for (int i = 0; i < 100; i++)
	{
		printf("creating the %d-th prime array\n", i);
		create_prime_array();
	}
}