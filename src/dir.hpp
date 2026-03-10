#pragma once


#include <stdint.h>

#include <sys/types.h>


// Directory
class Dir
{
public:
	// Open
	Dir(const char *path);
	// Close
	~Dir();

	// Read many directory entries (linux_dirent64 from https://man7.org/linux/man-pages/man2/getdents64.2.html)
	//
	// Returns the number of bytes or -1 for an error and errno is set
	ssize_t read(uint8_t &buffer, size_t buffer_size);

	// Whether it opened
	operator bool() const;

private:
	int m_file;
};
