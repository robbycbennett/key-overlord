#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/uinput.h>
#include <sys/epoll.h>

#include "config.hpp"
#include "dir.hpp"
#include "key_name.hpp"
#include "physical_keyboard.hpp"
#include "virtual_keyboard.hpp"


// The same as Wayland wl_keyboard_key_state
enum KeyState {
	KeyStateRelease,
	KeyStatePress,
	KeyStateRepeat,
};


#define FAIL_INNER(message) "keyoverlord: " message "\n"
#define FAIL(message) return fwrite(FAIL_INNER(message), 1, sizeof(FAIL_INNER(message)) - 1, stderr), 1;

#define PHYSICAL_DEVICE_DIRECTORY "/dev/input/by-path/"


static bool running = true;


static void handle_input_event(VirtualKeyboard &keyboard, const input_event &event)
{
	if (event.type != EV_KEY)
		return;
	if (event.value < KeyStateRelease || event.value > KeyStateRepeat)
		return;

	switch (event.value) {
		case KeyStateRelease:
			printf("Released %s\n", get_key_name(event.code));
			break;
		case KeyStatePress:
			printf(" Pressed %s\n", get_key_name(event.code));
			break;
		case KeyStateRepeat:
			printf("Repeated %s\n", get_key_name(event.code));
			break;
		default:
			break;
	}
}


static void handle_signal(int)
{
	running = false;
}


// Whether the string ends in "0-event-kbd"
static bool is_physical_device(const char *string, size_t length)
{
	constexpr size_t SUBSTR_LENGTH = sizeof(PHYSICAL_DEVICE_NAME_END) - 1;
	if (length < SUBSTR_LENGTH)
		return false;
	return memcmp(string + length - SUBSTR_LENGTH, PHYSICAL_DEVICE_NAME_END, SUBSTR_LENGTH) == 0;
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
	VirtualKeyboard virtual_keyboard;
	if (not virtual_keyboard.open())
		FAIL("Failed to create a virtual keyboard")

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
	PhysicalKeyboard physical_keyboards[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir)
			FAIL("Failed to open " PHYSICAL_DEVICE_DIRECTORY)

		char path[256] = PHYSICAL_DEVICE_DIRECTORY;

		// Each file
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
			PhysicalKeyboard &physical = physical_keyboards[i];
			if (not physical.open(path))
				FAIL("Failed to open a physical keyboard device")
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

	// Wait for the next general event
	while (running) {
		int event_count = epoll_wait(epoll_file, epoll_events, MAX_KEYBOARDS, -1);
		if (event_count < 0)
			continue;

		// Each general event
		for (int i = 0; i < event_count; i++) {
			epoll_event &general_event = epoll_events[i];
			int file = general_event.data.fd;

			// Get input events
			ssize_t bytes = read(file, input_events, INPUT_EVENT_COUNT * sizeof(input_event));
			if (bytes < 0)
				continue;

			// Each input event
			const size_t input_event_count = static_cast<size_t>(bytes) / sizeof(input_event);
			for (size_t j = 0; j < input_event_count; j++)
				handle_input_event(virtual_keyboard, input_events[j]);

			// for (uint8_t j = 0; j < INPUT_EVENT_COUNT; j++) {
			// 	input_event &event = input_events[j];
			// 	printf("%d %d %d\n", event.type, event.code, event.value);
			// }
			// TODO instead of this, remap and use the 2 events per iteration instead of 3
			virtual_keyboard.write(*input_events, INPUT_EVENT_COUNT);
		}
	}

	// TODO cleanup epoll_file
	// TODO cleanup epoll_events
	// epoll_ctl(epoll_file, EPOLL_CTL_DEL, file, &mEpollFileInfos[i]) == 0;
}
