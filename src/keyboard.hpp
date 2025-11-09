#pragma once


// A keyboard file
class Keyboard
{
public:
	// No file
	Keyboard() = default;

	// Close the file
	~Keyboard();

	// Close the file
	bool close();

	// Get the file
	int file();

	// Get exclusive access
	bool grab();

	// Open the file for read & write access, closing the old file
	bool open_physical(const char *path);
	// Open the file for write access, closing the old file
	bool open_virtual();

	// Get the next event, defined in <linux/input.h>
	bool read(struct input_event &event);

	// Release exclusive access
	bool ungrab();

	// Input some keys
	bool write(const struct input_event &events, size_t event_count);

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;
	bool m_virtual = false;

	Keyboard(const Keyboard &&) = delete;
	Keyboard operator=(const Keyboard &&) = delete;
};
