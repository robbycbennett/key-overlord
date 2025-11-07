#include <signal.h>
#include <string.h>

#include <linux/input.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "config.hpp"
#include "dir.hpp"
#include "error.hpp"
#include "keyboard.hpp"
#include "keyboard_state.hpp"
#include "output_events.hpp"


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

	// Prepare to listen to keyboard events
	int epoll_file = epoll_create1(0);
	if (epoll_file == -1)
		FAIL("Failed to create an event poll file")

	// Acquire physical keyboards and listen to them
	Keyboard physical_keyboards[MAX_KEYBOARDS];
	epoll_event epoll_events[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir)
			FAIL("Failed to open " PHYSICAL_DEVICE_DIRECTORY)

		char path[256] = PHYSICAL_DEVICE_DIRECTORY;

		uint8_t i = 0;
		for (const char *name = dir.read(); name && running; name = dir.read()) {
			size_t name_length = strlen(name);
			if (sizeof(PHYSICAL_DEVICE_DIRECTORY) + name_length > sizeof(path))
				FAIL("Path of physical keyboard device is too large")
			if (not is_physical_device(name, name_length))
				continue;
			Keyboard &physical = physical_keyboards[i];
			memcpy(path + sizeof(PHYSICAL_DEVICE_DIRECTORY) - 1, name, name_length);
			if (not physical.open_physical(path))
				FAIL("Failed to open a physical keyboard device")
			if (not physical.grab())
				FAIL("Failed to grab a physical keyboard device")
			if (epoll_ctl(epoll_file, EPOLL_CTL_ADD, physical.file(), epoll_events + i) == -1)
				FAIL("Failed to watch a physical keyboard device")
			i++;
			if (i >= MAX_KEYBOARDS)
				break;
		}
	}

	// TODO release all keys before grab
	// TODO watch for keyboard plug/unplug events

	static constexpr uint8_t INPUT_EVENT_COUNT = 3;
	input_event input_events[INPUT_EVENT_COUNT];

	while (true) {
		int event_count = epoll_wait(epoll_file, epoll_events, MAX_KEYBOARDS, -1);
		if (event_count < 0)
			FAIL("Failed to wait for keyboard events")
		printf("%d\n", event_count); // DEBUG
		for (int i = 0; i < event_count; i++) {
			int file = epoll_events[i].data.fd;
			printf("keyboard %d had %u events", file, epoll_events[i].events);
			// TODO read all events of the keyboard in 1 system call
			// ssize_t bytes = read(file, input_events, INPUT_EVENT_COUNT * sizeof(input_event));
			// TODO write
		}
	}

	// TODO cleanup epoll_file
	// TODO cleanup epoll_events
	// epoll_ctl(epoll_file, EPOLL_CTL_DEL, file, &mEpollFileInfos[i]) == 0;
}
