#pragma once


#include <stdint.h>

#include "key_span.hpp"


static constexpr uint16_t KEY_COUNT = 768;


// TODO consider a dual stack data structure instead (fixed on stack, unlimited on heap), which keeps track of the order of the pressed keys, eliminating hundreds of nearly pointless keys from iteration
// A bit for each of the 768 keys
class KeyboardState
{
public:
	constexpr KeyboardState() = default;

	constexpr KeyboardState(KeySpan keys)
	{
		for (size_t i = 0; i < keys.size; i++)
			set(keys.data[i]);
	}

	constexpr void clear(uint16_t key)
	{
		if (key > KEY_COUNT)
			return;
		m_data[key / 8] &= ~(1 << (key % 8));
	}

	constexpr bool get(uint16_t key)
	{
		if (key > KEY_COUNT)
			return false;
		return m_data[key / 8] & (1 << (key % 8));
	}

	constexpr void set(uint16_t key)
	{
		if (key > KEY_COUNT)
			return;
		m_data[key / 8] |= 1 << (key % 8);
	}

	constexpr bool operator==(const KeyboardState &other) const
	{
		for (uint8_t i = 0; i < KEY_GROUP_COUNT; i++)
			if (m_data[i] != other.m_data[i])
				return false;
		return true;
	}

private:
	static constexpr uint8_t KEY_GROUP_COUNT = KEY_COUNT / 8;

	uint8_t m_data[KEY_GROUP_COUNT] = {0};
};
