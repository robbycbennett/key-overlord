#include <signal.h>
#include <stdint.h>
#include <string.h>

#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "config.hpp"
#include "dir.hpp"
#include "error.hpp"
#include "keyboard.hpp"


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

	// TODO separate the keyboard into 2 classes

	// Acquire the virtual keyboard
	Keyboard virtual_keyboard;
	if (not virtual_keyboard.open_virtual())
		FAIL("Failed to create a virtual keyboard")
	int virtual_keyboard_file = virtual_keyboard.file();

	// Prepare to listen to keyboard events
	int epoll_file = epoll_create1(0);
	if (epoll_file == -1)
		FAIL("Failed to create an event poll file")

	// Configure what keyboard events to listen to
	epoll_event epoll_events[MAX_KEYBOARDS] = {
		epoll_event {
			.events = EPOLLIN,
			.data = epoll_data { .ptr = nullptr },
		},
		epoll_event {
			.events = EPOLLIN,
			.data = epoll_data { .ptr = nullptr },
		},
	};

	// Acquire physical keyboards and listen to them
	Keyboard physical_keyboards[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir)
			FAIL("Failed to open " PHYSICAL_DEVICE_DIRECTORY)

		char path[256] = PHYSICAL_DEVICE_DIRECTORY;

		uint8_t i = 0;
		for (const char *name = dir.read(); name && running; name = dir.read()) {
			// Skip everything but physical keyboards
			size_t name_length = strlen(name);
			if (not is_physical_device(name, name_length))
				continue;

			// Get the path to the keyboard
			if (sizeof(PHYSICAL_DEVICE_DIRECTORY) + name_length > sizeof(path))
				FAIL("Path of physical keyboard device is too large")
			memcpy(path + sizeof(PHYSICAL_DEVICE_DIRECTORY) - 1, name, name_length);

			// Acquire and grab the keyboard
			Keyboard &physical = physical_keyboards[i];
			if (not physical.open_physical(path))
				FAIL("Failed to open a physical keyboard device")
			if (not physical.grab())
				FAIL("Failed to grab a physical keyboard device")
			// Watch the keyboard and remember the file descriptor
			epoll_events[i].data.fd = physical.file();
			if (epoll_ctl(epoll_file, EPOLL_CTL_ADD, physical.file(), epoll_events + i) == -1)
				FAIL("Failed to watch a physical keyboard device")
			i++;
			if (i >= MAX_KEYBOARDS)
				break;
		}
	}

	// TODO wait until the device node is available (libevdev fetch_syspath_and_devnode) and release all keys before grab

	// TODO watch for keyboard plug/unplug events

	static constexpr uint8_t INPUT_EVENT_COUNT = 3;
	input_event input_events[INPUT_EVENT_COUNT];

	while (running) {
		int event_count = epoll_wait(epoll_file, epoll_events, MAX_KEYBOARDS, -1);
		if (event_count < 0)
			continue;

		for (int i = 0; i < event_count; i++) {
			epoll_event &event = epoll_events[i];
			int file = event.data.fd;
			ssize_t bytes = read(file, input_events, INPUT_EVENT_COUNT * sizeof(input_event));
			if (bytes != INPUT_EVENT_COUNT * sizeof(input_event)) {
				printf("warning: %zd bytes read\n", bytes); // DEBUG
				continue;
			}
			// for (uint8_t j = 0; j < INPUT_EVENT_COUNT; j++) {
			// 	input_event &event = input_events[j];
			// 	printf("%d %d %d\n", event.type, event.code, event.value);
			// }
			// TODO instead of this, remap and use the 2 events per iteration instead of 3
			bytes = write(virtual_keyboard_file, input_events, INPUT_EVENT_COUNT * sizeof(input_event));
		}
	}

	// TODO cleanup epoll_file
	// TODO cleanup epoll_events
	// epoll_ctl(epoll_file, EPOLL_CTL_DEL, file, &mEpollFileInfos[i]) == 0;
}
