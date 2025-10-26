#include <dirent.h>

#include "dir.hpp"


Dir::Dir(const char *path)
{
	m_dir = path ? opendir(path) : nullptr;
}


Dir::~Dir()
{
	if (m_dir)
		closedir(static_cast<DIR *>(m_dir));
}


const char *Dir::read()
{
	if (not m_dir)
		return nullptr;
	dirent *file = readdir(static_cast<DIR *>(m_dir));
	return file ? file->d_name : nullptr;
}


Dir::operator bool() const
{
	return m_dir;
}
