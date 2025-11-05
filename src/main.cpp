#include <signal.h>
#include <string.h>

#include <linux/input.h>
#include <unistd.h>

#include "config.hpp"
#include "dir.hpp"
#include "error.hpp"
#include "keyboard.hpp"
#include "keyboard_state.hpp"


#define PHYSICAL_DEVICE_DIRECTORY "/dev/input/by-path/"

static bool running = true;


// Whether the string ends in "0-event-kbd"
static bool is_physical_device(const char *string, size_t length)
{
	constexpr size_t MIN_LENGTH = sizeof(PHYSICAL_DEVICE_NAME_END) - 1;
	if (length < MIN_LENGTH)
		return false;
	return strcmp(string + length - MIN_LENGTH, PHYSICAL_DEVICE_NAME_END) == 0;
}


static void handle_signal(int)
{
	running = false;
}


int main()
{
	// Fail if not root
	if (getuid() != 0)
		FAIL("Run as root to acquire keyboards")

	// Handle signals gracefully
	signal(SIGHUP,  handle_signal);
	signal(SIGINT,  handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGABRT, handle_signal);
	signal(SIGTERM, handle_signal);

	// Acquire the virtual keyboard
	Keyboard virtual_keyboard;
	if (not virtual_keyboard.open_virtual())
		FAIL("Failed to create a virtual keyboard")

	// Acquire physical keyboards
	Keyboard physical_keyboards[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir)
			FAIL("Failed to open " PHYSICAL_DEVICE_DIRECTORY)

		char path[256] = PHYSICAL_DEVICE_DIRECTORY;

		uint8_t i = 0;
		for (const char *file = dir.read(); file && running; file = dir.read()) {
			size_t filename_length = strlen(file);
			if (sizeof(PHYSICAL_DEVICE_DIRECTORY) + filename_length > sizeof(path))
				FAIL("Path of physical keyboard device is too large")
			if (not is_physical_device(file, filename_length))
				continue;
			Keyboard &physical = physical_keyboards[i];
			memcpy(path + sizeof(PHYSICAL_DEVICE_DIRECTORY) - 1, file, filename_length);
			if (not physical.open_physical(path))
				FAIL("Failed to open a physical keyboard device")
			if (not physical.grab())
				FAIL("Failed to grab a physical keyboard device")
			i++;
			if (i >= MAX_KEYBOARDS)
				break;
		}
	}

	// Initialize the event array
	// TODO do it at compile time mwhahahaha
	static constexpr uint16_t EVENT_COUNT = KEY_COUNT * 2;
	input_event events[EVENT_COUNT];
	for (size_t i = 0; i < EVENT_COUNT - 1; i += 2) {
		events[i].type      = EV_KEY;
		events[i + 1].type  = EV_SYN;
		events[i + 1].code  = SYN_REPORT;
		events[i + 1].value = 0;
	}
}
