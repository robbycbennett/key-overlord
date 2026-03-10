#pragma once


#include <stddef.h>
#include <stdint.h>


class Dir;


// Borrowed directory entries
class DirIterator
{
public:
	DirIterator(Dir &dir, uint8_t &buffer, size_t buffer_size);

	const char *next_name();

private:
	Dir *m_dir;

	uint8_t *m_buffer;
	size_t m_buffer_size;

	size_t m_byte_position;
	size_t m_bytes_read;
};
