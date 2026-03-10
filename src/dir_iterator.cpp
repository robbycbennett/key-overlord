#include <sys/types.h>

#include "dir.hpp"
#include "dir_iterator.hpp"


// https://man7.org/linux/man-pages/man2/getdents.2.html
struct linux_dirent64
{
	// 64-bit inode number
	ino64_t d_ino;
	// Filesystem-specific value with no specific meaning to user space
	off64_t d_off;
	// Size of this dirent
	unsigned short d_reclen;
	// File type
	unsigned char d_type;
	// Filename (null-terminated)
	char d_name[];
};


DirIterator::DirIterator(Dir &dir, uint8_t &buffer, size_t buffer_size)
{
	m_dir = &dir;

	m_buffer = &buffer;
	m_buffer_size = buffer_size;

	m_byte_position = 0;
	m_bytes_read = 0;
}


const char *DirIterator::next_name()
{
	if (m_byte_position >= m_bytes_read) {
		ssize_t bytes_read = m_dir->read(*m_buffer, m_buffer_size);
		if (bytes_read <= 0) {
			return nullptr;
		}
		m_byte_position = 0;
		m_bytes_read = static_cast<size_t>(bytes_read);
	}

	struct linux_dirent64 *entry = reinterpret_cast<linux_dirent64 *>(m_buffer + m_byte_position);
	m_byte_position += entry->d_reclen;
	return entry->d_name;
}
