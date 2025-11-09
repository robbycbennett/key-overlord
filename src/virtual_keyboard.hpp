#pragma once


#include <stddef.h>


// An emulated keyboard
class VirtualKeyboard
{
public:
	// No file
	VirtualKeyboard() = default;

	// Close the file
	~VirtualKeyboard();

	// Remove the virtual keyboard and close the file
	bool close();

	// Get the file
	int file();

	// Open the file for write access, closing the old file
	bool open();

	// Input some key events, defined in <linux/input.h>
	bool write(const struct input_event &events, size_t event_count);

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;

	VirtualKeyboard(const VirtualKeyboard &&) = delete;
	VirtualKeyboard operator=(const VirtualKeyboard &&) = delete;
};
