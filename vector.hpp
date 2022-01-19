#ifndef SLAW_VECTOR_H
#define SLAW_VECTOR_H

#include "types.hpp"
#include "mem.hpp"
#include "util.hpp"
#include "math.hpp"

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
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	Vector(usize initial_size = min_capacity)
		: data(new T[initial_size]), size(0), capacity(initial_size) {}

	/**
	 * Constructs a new vector by taking a copy of an existing vector.
	 * The new vector will have the same size and capacity as the source.
	 *
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
	 *
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
	 *
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
	 *
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
	 *
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
	 */
	void
	realloc(usize new_capacity)
	{
		// Create a new array of the new capacity.

		T *new_data = new T[new_capacity];

		// We will copy the elements from the old array to the new one.

		usize elements_to_copy = min(size, new_capacity);

		for (usize i = 0; i < elements_to_copy; i++)
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

		// Update the size.

		size = vector.size;
	}

	/**
	 * Deletes all elements from the vector.
	 * The size and capacity are reset to 0.
	 *
	 * - Time complexity: O(1).
	 * - Space complexity: O(1).
	 */
	void
	clear()
	{
		// Reset the size and capacity.

		size = 0;
		capacity = 0;

		// Free the array.

		delete[] data;
		data = nullptr;
	}

	/**
	 * Returns the index of the first occurrence of a given element.
	 * Returns -1 if the element is not found.
	 * The vector will be searched from the provided index, or from
	 * the beginning of the vector if no index is provided.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	isize
	index_of(const T &element, usize index = 0) const
	{
		// Find the index of the element.

		for (isize i = index; i < size; i++)
		{
			if (data[i] == element)
			{
				return i;
			}
		}

		return -1;
	}

	/**
	 * Checks if the vector contains a given element.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	contains(const T &element) const
	{
		// Find the element.

		for (isize i = 0; i < size; i++)
		{
			if (data[i] == element)
			{
				// The element was found.

				return true;
			}
		}

		// The element was not found.

		return false;
	}

	/**
	 * Checks if the vector contains a given sequence of elements.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	bool
	contains(const Vector<T> &sequence) const
	{
		// If the sequence is longer than the vector,
		// the vector cannot possibly contain the sequence.

		if (sequence.size > size)
		{
			return false;
		}

		// Check if the sequence is contained in the vector.

		for (usize i = 0; i < size - sequence.size + 1; i++)
		{
			bool found = true;

			// For each index in the vector, we check if the
			// elements from this index are equal to the
			// elements from the sequence.

			for (usize j = 0; j < sequence.size; j++)
			{
				if (data[i + j] != sequence[j])
				{
					// We found elements that are not equal,
					// so we will advance to the next index.

					found = false;
					break;
				}
			}

			if (found)
			{
				// We found the sequence in the vector.

				return true;
			}
		}

		// The sequence is not contained in the vector.

		return false;
	}

	/**
	 * Rotates the elements on the array by a given shift size.
	 * Positive shifts will rotate the elements to the left,
	 * negative shifts will rotate the elements to the right.
	 * The elements will wrap around the array.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	void
	rotate(isize shift)
	{
		// Ensure the shift is within bounds of the size of the array.

		shift %= size;

		// Since the modulo operator might return a negative number,
		// we need to make sure that the shift is positive.

		if (shift < 0)
		{
			shift += size;
		}

		// We define the vector to have `n` "rings".
		// The i-th ring starts at the index `i`.
		//
		// The elements of each ring are `shift` indices apart from
		// each other, and we will go through all rings and move the
		// elements one place to the right within that ring.
		// If we do this for each ring, we are effectively rotating
		// the elements on the vector by `shift`, which is exactly what
		// we need to do.
		//
		// From the definition of the rings of the vector, it follows
		// that the number of rings in the vector is equal to the
		// greatest common divisor of `size` and `shift`.
		//
		// If we simply have one ring, we could return to the element
		// we started from before going over all elements of the vector.
		// Consider a vector of 6 elements:
		//
		// [0] [1] [2] [3] [4] [5]
		//  ╰───────^
		//          ╰───────^
		//  ^───────────────╯
		//
		// We would only rotate 0, 2 and 4, missing the ring 1, 3, 5.

		usize num_rings = gcd(size, (usize) shift);

		// Rotate the elements.

		for (usize i = 0; i < num_rings; i++)
		{
			// For each ring, we will move the elements one place
			// to the right. We start at the index of the ring.
			// We will save the element at the end of the ring,
			// so we can move it back to the beginning of the ring.

			T last_element = data[i];
			usize j = i;

			while (true)
			{
				// Compute the index of the next element in the
				// ring.

				usize next_index = j + shift;

				// If the next index is greater than the size of
				// the vector, we wrap around the vector.

				if (next_index >= size)
				{
					next_index -= size;
				}

				if (next_index == i)
				{
					// We have reached the beginning of the
					// ring. We are done with this ring.
					// Move the last element back to the
					// beginning of the ring.

					data[j] = last_element;
					break;
				}

				// Move the element to the next index.

				data[j] = data[next_index];
				j = next_index;
			}
		}
	}

	/**
	 * Reverses the order of the elements on the vector in-place.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(1).
	 */
	void
	reverse()
	{
		// Swap the elements at the left half of the array with their
		// corresponding elements at the right half of the array.

		for (usize i = 0; i < size / 2; i++)
		{
			swap(data[i], data[size - 1 - i]);
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

	/**
	 * Fills a new vector of a given size with a given value.
	 * The vector is resized to the given size.
	 *
	 * - Time complexity: O(n).
	 * - Space complexity: O(n).
	 */
	static Vector<T>
	fill(usize size, const T &value)
	{
		Vector<T> result(size);

		for (usize i = 0; i < size; i++)
		{
			result[i] = value;
		}

		return result;
	}
};
}; // namespace slaw

#endif