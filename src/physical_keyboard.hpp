#pragma once


// A real keyboard device
class PhysicalKeyboard
{
public:
	// No file
	PhysicalKeyboard() = default;
	// Open the file for read and write access
	PhysicalKeyboard(const char *path);

	// Close the file
	~PhysicalKeyboard();

	// Get exclusive access
	bool grab();

	// Get the next event, defined in <linux/input.h>
	bool read(struct input_event &event);

	// Release exclusive access
	bool ungrab();

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;
};
