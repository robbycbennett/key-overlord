#include <string.h>

#include "name.hpp"


void Name::clear()
{
	m_size = 0;
}


bool Name::equals(const char &name, size_t size) const
{
	return size == m_size and memcmp(&name, m_name, size) == 0;
}


void Name::set(const char &name, size_t size)
{
	if (size > NAME_MAX)
		return;
	m_size = size;
	memcpy(m_name, &name, size);
}
