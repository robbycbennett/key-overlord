#pragma once


#include <stddef.h>
#include <stdint.h>


// A view of an statically-allocated array of keys
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

	constexpr bool operator==(KeySpan other) const
	{
		return data == other.data and size == other.size;
	}
};
