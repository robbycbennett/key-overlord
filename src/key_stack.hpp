#pragma once


#include <stdint.h>


template<typename uint16_t>
class ReverseRange;


// Keeps track of pressed keys
class KeyStack
{
public:
	KeyStack();

	const uint16_t *begin() const;
	const uint16_t *end() const;

	ReverseRange<uint16_t> reverse_range();

	void push(uint16_t key);
	void remove(uint16_t key);

private:
	constexpr static uint8_t CAPACITY = 20;

	uint8_t m_size;
	uint16_t m_data[CAPACITY];
};
