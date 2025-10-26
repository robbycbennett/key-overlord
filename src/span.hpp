#pragma once


#include <stddef.h>


// A view of a contiguous collection of elements
template <typename T>
struct Span
{
	size_t size;
	T *data;

	Span()
	{}

	constexpr Span(size_t size_, T *data_)
	{
		size = size_;
		data = data_;
	}

	T* begin()
	{
		return data;
	}

	const T* begin() const
	{
		return data;
	}

	bool contains(const T &value) const
	{
		for (const T &other : *this)
			if (other == value)
				return true;
		return false;
	}

	T* end()
	{
		return data + size;
	}

	const T* end() const
	{
		return data + size;
	}

	bool ends_with(const T &value) const
	{
		if (size == 0)
			return false;
		return data[size - 1] == value;
	}

	bool ends_with(Span other) const
	{
		if (size < other.size)
			return false;
		size_t this_begin = size - other.size;
		for (size_t i = 0; i < other.size; i++)
			if (data[this_begin + i] != other.data[i])
				return false;
		return true;
	}

	bool starts_with(const T &value) const
	{
		if (size == 0)
			return false;
		return data[0] == value;
	}

	bool starts_with(Span other) const
	{
		if (size < other.size)
			return false;
		for (size_t i = 0; i < other.size; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}

	T& operator[](size_t index)
	{
		return data[index];
	}

	const T& operator[](size_t index) const
	{
		return data[index];
	}

	bool operator==(Span<const T> other) const
	{
		if (other.size != size)
			return false;

		if (other.data == data)
			return true;

		for (size_t i = 0; i < size; i++)
			if (other.data[i] != data[i])
				return false;

		return true;
	}
};
