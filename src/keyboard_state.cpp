#include "keyboard_state.hpp"


#ifdef DEBUG
	#include <assert.h>
	#define DEBUG_INDEX assert(key < KEY_COUNT);
#else
	#define DEBUG_INDEX
#endif


void KeyboardState::clear(uint16_t key)
{
	DEBUG_INDEX
	m_data[key / 8] &= ~(1 << (key % 8));
}


bool KeyboardState::get(uint16_t key)
{
	DEBUG_INDEX
	return m_data[key / 8] & (1 << (key % 8));
}


void KeyboardState::set(uint16_t key)
{
	DEBUG_INDEX
	m_data[key / 8] |= 1 << (key % 8);
}


bool KeyboardState::operator==(const KeyboardState &other) const
{
	for (uint8_t i = 0; i < KEY_GROUP_COUNT; i++)
		if (m_data[i] != other.m_data[i])
			return false;
	return true;
}
