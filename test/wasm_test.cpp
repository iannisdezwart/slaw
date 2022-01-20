#include "../slaw.hpp"

EXPORT("vec_test")
void
vec_test(usize size)
{
	slaw::Vector<int> vec;

	for (int i = 0; i < size; i++)
	{
		vec.push_back(i);
	}

	for (int i = 0; i < size; i++)
	{
		if (vec[i] != size - 1 - i)
		{
			slaw::eval("console.warn('vec_test() failed!')");
			return;
		}
	}

	slaw::eval("console.log('vec_test() passed!')");
}

EXPORT("string_test")
void
string_test()
{
	slaw::String s = "Hello, ";
	s += "world!";

	if (s != "Hello, world!")
	{
		slaw::eval("console.warn('string_test() failed!')");
		return;
	}

	slaw::eval("console.log('string_test() passed!')");
}

EXPORT("print_test")
void
print_test()
{
	slaw::print("Hello, world!");
}

EXPORT("sqrt_test")
f64
sqrt_test(f64 n)
{
	constexpr const f64 test = slaw::sqrt(2);
	return slaw::sqrt(n);
}