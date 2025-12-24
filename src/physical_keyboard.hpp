#pragma once


#include <stddef.h>
#include <stdint.h>


static constexpr size_t INPUT_EVENT_COUNT = 3;


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

	// Get up to 3 events which are defined in <linux/input.h>
	// returning the number of events or -1 on failure
	int8_t read(struct input_event (&event)[INPUT_EVENT_COUNT]);

	// Input some key events, defined in <linux/input.h>
	bool write(const struct input_event &events, size_t event_count);

	// Whether the file was opened
	operator bool() const;

private:
	int m_file = -1;

	PhysicalKeyboard(const PhysicalKeyboard &&) = delete;
	PhysicalKeyboard operator=(const PhysicalKeyboard &&) = delete;
};
