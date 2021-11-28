#include <bits/stdc++.h>
#include <stdio.h>
#include "types.hpp"
#include "mem.hpp"

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

	while ((u8 *) block < slaw::mem::heap_end)
	{
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
		printf("start = %p\n", block->get_start_ptr());
		printf("end   = %p\n", block->get_end_ptr());
		printf("off   = %lu\n", (u8 *) block - heap);
		printf("\n");

		block = (slaw::mem::HeapBlockHeader *) ((char *) block + sizeof(slaw::mem::HeapBlockHeader) + block->size);
	}

	printf("===========================================\n\n\n");
}

int main()
{
	heap = new u8[1024 * 1024];
	slaw::mem::heap_end = heap;

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

	// printf("heap = %p\n", heap);

	// debug();

	// printf("allocating 100 bytes for a\n");

	// void *a = slaw::mem::alloc(100);
	// debug();

	// printf("allocating 200 bytes for b\n");

	// void *b = slaw::mem::alloc(200);
	// debug();

	// printf("allocating 300 bytes for c\n");

	// void *c = slaw::mem::alloc(300);
	// debug();

	// printf("freeing a\n");

	// slaw::mem::free(a);
	// debug();

	// printf("allocating 50 bytes for d\n");

	// void *d = slaw::mem::alloc(50);
	// debug();

	// printf("allocating 34 bytes for e\n");

	// void *e = slaw::mem::alloc(34);
	// debug();

	// printf("freeing d\n");

	// slaw::mem::free(d);
	// debug();

	// printf("freeing e\n");

	// slaw::mem::free(e);
	// debug();

	// printf("freeing c\n");

	// slaw::mem::free(c);
	// debug();

	// slaw::mem::free(b);
	// debug();

	delete heap;
}