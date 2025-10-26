#pragma once


// Directory
class Dir
{
public:
	// Open
	Dir(const char *path);
	// Close
	~Dir();

	// Get the next file name or null at end
	// * Invalidated on next call of read
	// * Invalidated on directory close
	const char *read();

	// Whether it opened
	operator bool() const;

private:
	void *m_dir;
};
