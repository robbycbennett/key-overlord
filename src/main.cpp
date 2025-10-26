#include <signal.h>
#include <string.h>

#include <unistd.h>

#include "config.hpp"
#include "dir.hpp"
#include "error.hpp"
#include "physical_keyboard.hpp"


static bool running = true;


// Whether the string ends in "0-event-kbd"
static bool is_physical_device(const char *string)
{
	constexpr size_t MIN_LENGTH = sizeof(PHYSICAL_DEVICE_NAME_END) - 1;
	size_t length = strlen(string);
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
	if (getuid() != 0) {
		PRINT_ERROR("Run as root to acquire keyboards")
		return 1;
	}

	// Handle signals gracefully
	signal(SIGHUP,  handle_signal);
	signal(SIGINT,  handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGABRT, handle_signal);
	signal(SIGTERM, handle_signal);

	// Acquire keyboards
	PhysicalKeyboard physicals[MAX_KEYBOARDS];
	uint8_t keyboard_i = 0;
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir) {
			PRINT_ERROR("Failed to open " PHYSICAL_DEVICE_DIRECTORY)
			return 1;
		}

		for (const char *file = dir.read(); file && running; file = dir.read()) {
			if (not is_physical_device(file))
				continue;
			PhysicalKeyboard &physical = physicals[keyboard_i];
			if (not physical.open(file)) {
				PRINT_ERROR("Failed to open a physical keyboard device")
				return 1;
			}
			if (not physical.grab()) {
				PRINT_ERROR("Failed to grab a physical keyboard device")
				return 1;
			}
			if (keyboard_i >= MAX_KEYBOARDS)
				break;
			keyboard_i++;
		}
	}
}
