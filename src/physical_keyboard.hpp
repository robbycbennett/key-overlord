#pragma once


// A real keyboard device
class PhysicalKeyboard
{
public:
	// No file
	PhysicalKeyboard() = default;

	// Close the file
	~PhysicalKeyboard();

	// Close the file
	bool close();

	// Get exclusive access
	bool grab();

	// Open the file for read and write access, closing the old file
	bool open(const char *path);

	// Get the next event, defined in <linux/input.h>
	bool read(struct input_event &event);

	// Release exclusive access
	bool ungrab();

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;

	PhysicalKeyboard(const PhysicalKeyboard &&) = delete;
	PhysicalKeyboard operator=(const PhysicalKeyboard &&) = delete;
};
