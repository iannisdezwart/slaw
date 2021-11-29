#ifndef SLAW_VECTOR_H
#define SLAW_VECTOR_H

#include "types.hpp"
#include "mem.hpp"
#include "util.hpp"

namespace slaw
{
/**
 * Dynamic array implementation.
 * Stores elements of type `T` in a contiguous block of memory.
 * Allows constant-time access to elements, and constant-time
 * insertion and removal of elements at the back of the vector.
 */
template <typename T>
struct Vector
{
	// The minimum allowed capacity of a vector.
	// This value is used as a default value for the initial vector capacity
	// in the constructor. The vector will not shrink below this value.
	static const constexpr usize min_capacity = 16;

	// A pointer to the internal array of elements.
	// This pointer should NOT be tamprered with.
	// Only touch this if you know what you are doing.
	T *data;

	// The number of elements in the vector.
	// This value should not be tampered with.
	// Only touch this if you know what you are doing.
	usize size;

	// The maximum number of elements the vector can hold right now.
	// This value should not be tampered with.
	// Only touch this if you know what you are doing.
	usize capacity;

	/**
	 * Constructs a new vector of a given initial size.
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	Vector(usize initial_size = min_capacity)
		: data(new T[initial_size]), size(0), capacity(initial_size) {}

	/**
	 * Constructs a new vector by taking a copy of an existing vector.
	 * The new vector will have the same size and capacity as the source.
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	Vector(const Vector &source)
		: data(new T[source.capacity]), size(source.size),
		  capacity(source.capacity)
	{
		// Copy the elements over.

		for (usize i = 0; i < size; i++)
		{
			data[i] = source.data[i];
		}
	}

	/**
	 * Constructs a new vector by moving an existing vector.
	 * The new vector will have the same size and capacity as the source.
	 * The source vector will be emptied.
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	Vector(Vector &&source)
		: data(source.data), size(source.size), capacity(source.capacity)
	{
		// Empty the other vector.

		source.data = nullptr;
		source.size = 0;
		source.capacity = 0;
	}

	/**
	 * Copies a new vector into this vector.
	 * The new vector will have the same size and capacity as the source.
	 * The original vector elements will be overwritten.
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	Vector &
	operator=(const Vector &source)
	{
		// Handle self-assignment.

		if (this == &source)
		{
			return *this;
		}

		// Delete the old data.

		delete[] data;

		// Set the new size and capacity.

		size = source.size;
		capacity = source.capacity;
		data = new T[capacity];

		// Copy the elements over from the source vector.

		for (usize i = 0; i < size; i++)
		{
			data[i] = source.data[i];
		}

		return *this;
	}

	/**
	 * Moves a new vector into this vector.
	 * The new vector will have the same size and capacity as the original.
	 * The source vector will be emptied and the original vector elements
	 * will be overwritten.
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	Vector &
	operator=(Vector &&source)
	{
		// Handle self-assignment.

		if (this == &source)
		{
			return *this;
		}

		// Delete the old data.

		delete[] data;

		// Steal the data, size and capacity.

		data = source.data;
		size = source.size;
		capacity = source.capacity;

		// Empty the source vector.

		source.data = nullptr;
		source.size = 0;
		source.capacity = 0;

		return *this;
	}

	/**
	 * Destructs the vector.
	 * Frees the internal array used by the vector.
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	~Vector()
	{
		delete[] data;
	}

	/**
	 * Reallocates the internal array to a new capacity.
	 * The given new capacity must be greater than or equal to the current
	 * number of elements in the vector.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(n).
	 *
	 * WARNING: If the new capacity is less than
	 * the size of the vector, BEHAVIOUR IS UNDEFINED.
	 */
	void
	realloc(usize new_capacity)
	{
		// Create a new array of the new capacity.

		T *new_data = new T[new_capacity];

		// We will copy the elements from the old array to the new one.

		for (usize i = 0; i < size; i++)
		{
			new_data[i] = data[i];
		}

		// Free the old array, and update the data pointer and capacity.

		delete[] data;
		data = new_data;
		capacity = new_capacity;
	}

	/**
	 * Returns a read-only reference to an element at a given index.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the given index is greater than or equal to the size of
	 * the vector, BEHAVIOUR IS UNDEFINED.
	 */
	const T &
	operator[](usize index)
	const
	{
		return data[index];
	}

	/**
	 * Returns a read-write reference to an element at a given index.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the given index is greater than or equal to the size of
	 * the vector, BEHAVIOUR IS UNDEFINED.
	 */
	T &
	operator[](usize index)
	{
		return data[index];
	}

	/**
	 * Returns a read-only reference to the first element in the vector.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the vector is empty, BEHAVIOUR IS UNDEFINED.
	 */
	const T &
	front()
	const
	{
		return *data;
	}

	/**
	 * Returns a read-write reference to the first element in the vector.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the vector is empty, BEHAVIOUR IS UNDEFINED.
	 */
	T &
	front()
	{
		return *data;
	}

	/**
	 * Returns a read-only reference to the last element in the vector.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the vector is empty, BEHAVIOUR IS UNDEFINED.
	 */
	const T &
	back()
	const
	{
		return data[size - 1];
	}

	/**
	 * Returns a read-write reference to the last element in the vector.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the vector is empty, BEHAVIOUR IS UNDEFINED.
	 */
	T &
	back()
	{
		return data[size - 1];
	}

	/**
	 * Reserves space for a given number of extra elements.
	 * The new capacity is updated to fit at least
	 * the given number of elements.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(n).
	 */
	void
	reserve(usize extra_size)
	{
		// If the vector fits the extra elements,
		// we will not have to do anything and return.

		if (size + extra_size <= capacity)
		{
			return;
		}

		// Otherwise, we need to reallocate the array.
		// We will double the capacity until it fits.
		// We can already multiply the capacity by 2 once,
		// because the capacity is guaranteed not to fit
		// the extra elements.

		usize new_size = size + extra_size;
		usize new_capacity = capacity * 2;

		while (new_size > new_capacity)
		{
			new_capacity *= 2;
		}

		// Reallocate the array.

		realloc(new_capacity);
	}

	/**
	 * Appends an element to the back of the vector.
	 * If the vector is full, it will be reallocated.
	 *
	 * - Time complexity: O(1) on average.
	 * - Space complexity: O(1).
	 */
	void
	push_back(const T &element)
	{
		// If the vector is full, reallocate it.

		if (size == capacity)
		{
			realloc(capacity * 2);
		}

		// Append the element.

		data[size++] = element;
	}

	/**
	 * Appends an element to the back of the vector. The element is moved.
	 * If the vector is full, it will be reallocated.
	 *
	 * - Time complexity: O(1) on average.
	 * - Space complexity: O(1).
	 */
	void
	push_back(T &&element)
	{
		// If the vector is full, reallocate it.

		if (size == capacity)
		{
			realloc(capacity * 2);
		}

		// Append the element.

		data[size++] = move(element);
	}

	/**
	 * Removes the last element from the vector.
	 * A copy of the element is returned.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 *
	 * WARNING: If the vector is empty, BEHAVIOUR IS UNDEFINED.
	 */
	T
	pop_back()
	{
		// Return the last element and remove it.

		return data[--size];
	}

	/**
	 * Attaches a vector to the back of this vector.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	void
	attach(const Vector<T> &vector)
	{
		// Reserve space for the new elements.

		reserve(vector.size);

		// Append the elements from the other vector to this vector.

		for (usize i = 0; i < vector.size; i++)
		{
			data[size++] = vector[i];
		}
	}

	/**
	 * Iterator implementation for the vector.
	 * Allows
	 */
	template <bool IsConst>
	struct IteratorBase
	{
		// A pointer to the current element.
		T *ptr;

		/**
		 * Creates an iterator for a vector starting at a given element.
		 */
		IteratorBase(T *ptr) : ptr(ptr) {}

		/**
		 * Returns the value at the current iterator position.
		 */
		inline T &
		operator*()
		{
			return *ptr;
		}

		/**
		 * Dereferences the current iterator position.
		 */
		inline T *
		operator->()
		{
			return ptr;
		}

		/**
		 * Prefix increment operator. Advances the iterator to the
		 * next element on the vector.
		 */
		inline IteratorBase &
		operator++()
		{
			ptr++;
			return *this;
		}

		/**
		 * Prefix decrement operator. Moves the iterator to the
		 * previous element on the vector.
		 */
		inline IteratorBase &
		operator--()
		{
			ptr--;
			return *this;
		}

		/**
		 * Equality operator.
		 */
		inline bool
		operator==(const IteratorBase &other)
		{
			return ptr == other.ptr;
		}

		/**
		 * Inequality operator.
		 */
		inline bool
		operator!=(const IteratorBase &other)
		{
			return ptr != other.ptr;
		}

		/**
		 * Less than operator.
		 */
		inline bool
		operator<(const IteratorBase &other)
		{
			return ptr < other.ptr;
		}
	};

	using Iterator = IteratorBase<false>;
	using ConstIterator = IteratorBase<true>;

	/**
	 * Returns a read-write iterator to the first element of the vector.
	 */
	Iterator
	begin()
	{
		return Iterator(data);
	}

	/**
	 * Returns a read-only iterator to the first element of the vector.
	 */
	ConstIterator
	cbegin()
	const
	{
		return ConstIterator(data);
	}

	/**
	 * Returns a read-write iterator to the element
	 * after the last element of the vector.
	 */
	Iterator
	end()
	{
		return Iterator(data + size);
	}

	/**
	 * Returns a read-only iterator to the element
	 * after the last element of the vector.
	 */
	ConstIterator
	cend()
	const
	{
		return ConstIterator(data + size);
	}

	/**
	 * Returns a read-write reverse iterator to
	 * the first element of the vector.
	 */
	Iterator
	rbegin()
	{
		return Iterator(data + size - 1);
	}

	/**
	 * Returns a read-only reverse iterator to
	 * the first element of the vector.
	 */
	ConstIterator
	crbegin()
	const
	{
		return ConstIterator(data + size - 1);
	}

	/**
	 * Returns a read-write reverse iterator to
	 * the element after the last element of the vector.
	 */
	Iterator
	rend()
	{
		return Iterator(data - 1);
	}

	/**
	 * Returns a read-only reverse iterator to
	 * the element after the last element of the vector.
	 */
	ConstIterator
	crend()
	const
	{
		return ConstIterator(data - 1);
	}
};
}; // namespace slaw

#endif