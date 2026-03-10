#pragma once


#include <stddef.h>

#include <linux/limits.h>


// A statically-allocated file name
class Name
{
public:
	Name() = default;

	void clear();

	bool equals(const char &name, size_t size) const;

	void set(const char &name, size_t size);

private:
	size_t m_size = 0;
	char m_name[NAME_MAX + 1];
};
