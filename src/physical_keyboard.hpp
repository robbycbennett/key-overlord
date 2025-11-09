#pragma once


#include <stddef.h>


// An actual keyboard
class PhysicalKeyboard
{
public:
	// No file
	PhysicalKeyboard() = default;

	// Close the file
	~PhysicalKeyboard();

	// Release exclusive access and close the file
	bool close();

	// Get the file
	int file();

	// Open and grab the file for exclusive read/write access, closing the old file
	bool open(const char *path);

	// Get the next event, defined in <linux/input.h>
	bool read(struct input_event &event);

	// Input some key events, defined in <linux/input.h>
	bool write(const struct input_event &events, size_t event_count);

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;

	PhysicalKeyboard(const PhysicalKeyboard &&) = delete;
	PhysicalKeyboard operator=(const PhysicalKeyboard &&) = delete;
};
