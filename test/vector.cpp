#include <iostream>
#include "../types.hpp"
#include "../vector.hpp"

u8 __heap_base; // We don't touch this.

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

int
main()
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