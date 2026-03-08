#pragma once


#include <stdint.h>

#include "key_span.hpp"


static constexpr uint16_t KEY_COUNT = 768;


// A bit for each of the 768 keys
class KeyboardState
{
public:
	constexpr KeyboardState() = default;

	constexpr KeyboardState(KeySpan keys)
	{
		const uint16_t *key_end = keys.data + keys.size;
		for (const uint16_t *key = keys.data; key < key_end; key++)
			set(*key);
	}

	constexpr void clear(uint16_t key)
	{
		if (key > KEY_COUNT)
			return;
		m_data[key / BITS_PER_GROUP] &= ~(static_cast<uint64_t>(1) << (key % BITS_PER_GROUP));
	}

	constexpr bool get(uint16_t key)
	{
		if (key > KEY_COUNT)
			return false;
		return m_data[key / BITS_PER_GROUP] & (static_cast<uint64_t>(1) << (key % BITS_PER_GROUP));
	}

	constexpr void set(uint16_t key)
	{
		if (key > KEY_COUNT)
			return;
		m_data[key / BITS_PER_GROUP] |= static_cast<uint64_t>(1) << (key % BITS_PER_GROUP);
	}

	constexpr bool operator==(const KeyboardState &other) const
	{
		const uint64_t *self_i = m_data;
		const uint64_t *other_i = other.m_data;
		const uint64_t *self_end = m_data + KEY_GROUP_COUNT;
		for (; self_i != self_end; self_i++, other_i++)
			if (*self_i != *other_i)
				return false;
		return true;
	}

private:
	static constexpr uint64_t BITS_PER_GROUP = 64;
	static constexpr uint64_t KEY_GROUP_COUNT = KEY_COUNT / BITS_PER_GROUP;

	uint64_t m_data[KEY_GROUP_COUNT] = {0};

	static_assert(BITS_PER_GROUP == 8 * sizeof(m_data[0]));
};
