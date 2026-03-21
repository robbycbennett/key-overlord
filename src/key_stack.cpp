#include "key_stack.hpp"
#include "reverse_range.hpp"


KeyStack::KeyStack()
{
	m_size = 0;
}


const uint16_t *KeyStack::begin() const
{
	return m_data;
}


const uint16_t *KeyStack::end() const
{
	return m_data + m_size;
}


ReverseRange<uint16_t> KeyStack::reverse_range()
{
	return ReverseRange<uint16_t>(m_data + m_size - 1, m_data - 1);
}


void KeyStack::push(uint16_t key)
{
	if (m_size >= CAPACITY) {
		return;
	}

	m_data[m_size] = key;
	m_size += 1;
}


void KeyStack::remove(uint16_t key)
{
	uint16_t *i = m_data;
	uint16_t *end = &m_data[m_size];
	for (; i != end; i++) {
		if (*i != key) {
			continue;
		}
		end--;
		for (; i != end; i++) {
			*i = *(i + 1);
		}
		m_size--;
		return;
	}
}
