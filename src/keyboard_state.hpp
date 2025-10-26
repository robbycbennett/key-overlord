#pragma once


#include <stdint.h>


static constexpr uint16_t KEY_COUNT = 768;


// A bit for each of the 768 keys
class KeyboardState
{
public:
	void clear(uint16_t key);
	bool get(uint16_t key);
	void set(uint16_t key);

private:
	uint8_t m_data[KEY_COUNT / 8] = {0};
};
