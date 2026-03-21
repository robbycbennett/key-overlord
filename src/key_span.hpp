#pragma once


#include <stddef.h>
#include <stdint.h>

#include "reverse_range.hpp"


// A read-only view of an statically-allocated array of keys
struct KeySpan
{
	const uint16_t *data;
	size_t size;

	constexpr const uint16_t *begin() const
	{
		return data;
	}

	constexpr const uint16_t *end() const
	{
		return data + size;
	}

	constexpr bool is_empty() const
	{
		return size == 0;
	}

	constexpr uint16_t last() const
	{
		return data[size - 1];
	}

	constexpr ReverseRange<uint16_t> reverse_range() const
	{
		return ReverseRange<uint16_t>(data + size - 1, data - 1);
	}

	constexpr bool operator==(KeySpan other) const
	{
		return data == other.data and size == other.size;
	}
};
