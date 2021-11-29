#include <iostream>
#include <string>
#include <utility>
#include <stdio.h>
#include <vector>
#include "../types.hpp"
#include "../mem.hpp"
#include "virtual_heap.hpp"

u8 __heap_base; // We don't touch this.
u8 *heap;

void
debug()
{
	printf("===========================================\n");
	printf("first_free_block = %p\n", slaw::mem::first_free_block);
	printf("heap_end         = %p\n", slaw::mem::heap_end);
	printf("\nBlocks:\n\n");

	auto block = (slaw::mem::HeapBlockHeader *) heap;
	std::vector<int> a;
	a.begin();

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
		printf("size  = %d\n", block->size);
		printf("prev  = %p\n", block->prev_block);
		printf("start = %p\n", block->get_start_ptr());
		printf("end   = %p\n", block->get_end_ptr());
		printf("off   = %lu\n", (u8 *) block - heap);

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

int main()
{
	heap = create_heap(1024 * 1024);

	std::string line;

	while (true)
	{
		debug();

		std::cout << "(q/a <size>/f <ptr>) >>> ";

		char c;
		std::cin >> c;

		switch (c)
		{
			case 'q':
				exit(0);

			case 'a':
			{
				usize size;
				std::cin >> size;

				void *ptr = slaw::mem::alloc(size);
				printf("Allocated %u bytes: %p\n",
					size, ptr);
				break;
			}

			case 'f':
			{
				void *ptr;
				std::cin >> ptr;

				slaw::mem::free(ptr);
				printf("Freed %p\n", ptr);
				break;
			}
		}
	}

	delete heap;
}