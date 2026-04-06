#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

#include "dir.hpp"


Dir::Dir(const char *path)
{
	m_file = path ? open(path, O_RDONLY | O_DIRECTORY) : -1;
}


Dir::~Dir()
{
	if (m_file != -1) {
		close(m_file);
	}
}


ssize_t Dir::read(uint8_t &buffer, size_t buffer_size)
{
	return getdents64(m_file, (struct dirent *) (void *) &buffer, buffer_size);
}


Dir::operator bool() const
{
	return m_file;
}
