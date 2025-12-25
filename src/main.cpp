#include <span>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <sys/epoll.h>

#include "config.hpp"
#include "dir.hpp"
#include "key_name.hpp"
#include "key_state.hpp"
#include "keyboard_state.hpp"
#include "mapping.hpp"
#include "output_events.hpp"
#include "physical_keyboard.hpp"
#include "virtual_keyboard.hpp"


#define APP_NAME "keyoverlord"
#define APP_VERSION "1.0.0"

#define FAIL_INNER(message) APP_NAME ": " message "\n"
#define FAIL(message) return fwrite(FAIL_INNER(message), 1, sizeof(FAIL_INNER(message)) - 1, stderr), 1;

#define MAPPING(array) \
	Mapping( \
		KeySpan {array##_INPUT, sizeof(array##_INPUT) / sizeof(uint16_t)}, \
		KeySpan {array##_OUTPUT, sizeof(array##_OUTPUT) / sizeof(uint16_t)} \
	)

#define PHYSICAL_DEVICE_DIRECTORY "/dev/input/by-path/"

#define SUCCEED_INNER(message) message "\n"
#define SUCCEED(message) return fwrite(SUCCEED_INNER(message), 1, sizeof(SUCCEED_INNER(message)) - 1, stdout), 1;


static constexpr const uint16_t SELECT_ALL_INPUT[] =
	{KEY_RIGHTALT, KEY_A};
static constexpr const uint16_t SELECT_ALL_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_A};

static constexpr const uint16_t CUT_INPUT[] =
	{KEY_RIGHTALT, KEY_X};
static constexpr const uint16_t CUT_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_X};
static constexpr const uint16_t COPY_INPUT[] =
	{KEY_RIGHTALT, KEY_C};
static constexpr const uint16_t COPY_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_C};
static constexpr const uint16_t PASTE_INPUT[] =
	{KEY_RIGHTALT, KEY_V};
static constexpr const uint16_t PASTE_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_V};

static constexpr const uint16_t DELETE_INPUT[] =
	{KEY_RIGHTALT, KEY_D};
static constexpr const uint16_t DELETE_OUTPUT[] =
	{KEY_DELETE};

static constexpr const uint16_t FIND_INPUT[] =
	{KEY_RIGHTALT, KEY_F};
static constexpr const uint16_t FIND_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_F};
static constexpr const uint16_t GO_TO_INPUT[] =
	{KEY_RIGHTALT, KEY_G};
static constexpr const uint16_t GO_TO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_G};

static constexpr const uint16_t OPEN_INPUT[] =
	{KEY_LEFTALT, KEY_O};
static constexpr const uint16_t OPEN_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_O};
static constexpr const uint16_t SAVE_INPUT[] =
	{KEY_RIGHTALT, KEY_S};
static constexpr const uint16_t SAVE_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_S};
static constexpr const uint16_t PRINT_INPUT[] =
	{KEY_LEFTALT, KEY_P};
static constexpr const uint16_t PRINT_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_P};

static constexpr const uint16_t UNDO_INPUT[] =
	{KEY_LEFTALT, KEY_U};
static constexpr const uint16_t UNDO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_Z};
static constexpr const uint16_t REDO_INPUT[] =
	{KEY_LEFTALT, KEY_I};
static constexpr const uint16_t REDO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_Y};

static constexpr const uint16_t OPEN_TAB_INPUT[] =
	{KEY_LEFTALT, KEY_N};
static constexpr const uint16_t OPEN_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_T};
static constexpr const uint16_t CLOSE_TAB_INPUT[] =
	{KEY_LEFTALT, KEY_M};
static constexpr const uint16_t CLOSE_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_W};
static constexpr const uint16_t REOPEN_TAB_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_N};
static constexpr const uint16_t REOPEN_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTSHIFT, KEY_T};

static constexpr const uint16_t PLAY_PAUSE_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_SPACE};
static constexpr const uint16_t PLAY_PAUSE_SONG_ALT_OUTPUT[] =
	{KEY_PLAYPAUSE};
static constexpr const uint16_t PLAY_PAUSE_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_SPACE};
static constexpr const uint16_t PLAY_PAUSE_SONG_CTRL_OUTPUT[] =
	{KEY_PLAYPAUSE};
static constexpr const uint16_t PREVIOUS_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_LEFT};
static constexpr const uint16_t PREVIOUS_SONG_ALT_OUTPUT[] =
	{KEY_PREVIOUSSONG};
static constexpr const uint16_t PREVIOUS_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_LEFT};
static constexpr const uint16_t PREVIOUS_SONG_CTRL_OUTPUT[] =
	{KEY_PREVIOUSSONG};
static constexpr const uint16_t NEXT_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_RIGHT};
static constexpr const uint16_t NEXT_SONG_ALT_OUTPUT[] =
	{KEY_NEXTSONG};
static constexpr const uint16_t NEXT_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_RIGHT};
static constexpr const uint16_t NEXT_SONG_CTRL_OUTPUT[] =
	{KEY_NEXTSONG};
static constexpr const uint16_t VOLUME_UP_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_UP};
static constexpr const uint16_t VOLUME_UP_ALT_OUTPUT[] =
	{KEY_VOLUMEUP};
static constexpr const uint16_t VOLUME_UP_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_UP};
static constexpr const uint16_t VOLUME_UP_CTRL_OUTPUT[] =
	{KEY_VOLUMEUP};
static constexpr const uint16_t VOLUME_DOWN_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_DOWN};
static constexpr const uint16_t VOLUME_DOWN_ALT_OUTPUT[] =
	{KEY_VOLUMEDOWN};
static constexpr const uint16_t VOLUME_DOWN_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_DOWN};
static constexpr const uint16_t VOLUME_DOWN_CTRL_OUTPUT[] =
	{KEY_VOLUMEDOWN};

static constexpr const uint16_t MOVE_LEFT_INPUT[] =
	{KEY_LEFTALT, KEY_J};
static constexpr const uint16_t MOVE_LEFT_OUTPUT[] =
	{KEY_LEFT};
static constexpr const uint16_t MOVE_DOWN_INPUT[] =
	{KEY_LEFTALT, KEY_K};
static constexpr const uint16_t MOVE_DOWN_OUTPUT[] =
	{KEY_DOWN};
static constexpr const uint16_t MOVE_UP_INPUT[] =
	{KEY_LEFTALT, KEY_L};
static constexpr const uint16_t MOVE_UP_OUTPUT[] =
	{KEY_UP};
static constexpr const uint16_t MOVE_RIGHT_INPUT[] =
	{KEY_LEFTALT, KEY_SEMICOLON};
static constexpr const uint16_t MOVE_RIGHT_OUTPUT[] =
	{KEY_RIGHT};

static constexpr const uint16_t MOVE_TO_WORD_START_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_J};
static constexpr const uint16_t MOVE_TO_WORD_START_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_LEFT};
static constexpr const uint16_t MOVE_TO_WORD_END_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_SEMICOLON};
static constexpr const uint16_t MOVE_TO_WORD_END_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_RIGHT};

static constexpr const uint16_t MOVE_TO_LINE_START_INPUT[] =
	{KEY_LEFTALT, KEY_H};
static constexpr const uint16_t MOVE_TO_LINE_START_OUTPUT[] =
	{KEY_HOME};
static constexpr const uint16_t MOVE_TO_LINE_END_INPUT[] =
	{KEY_LEFTALT, KEY_APOSTROPHE};
static constexpr const uint16_t MOVE_TO_LINE_END_OUTPUT[] =
	{KEY_END};

static constexpr const uint16_t MOVE_PAGE_DOWN_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_K};
static constexpr const uint16_t MOVE_PAGE_DOWN_OUTPUT[] =
	{KEY_PAGEDOWN};
static constexpr const uint16_t MOVE_PAGE_UP_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_L};
static constexpr const uint16_t MOVE_PAGE_UP_OUTPUT[] =
	{KEY_PAGEUP};

static constexpr const uint16_t SELECT_LEFT_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_J};
static constexpr const uint16_t SELECT_LEFT_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFT};
static constexpr const uint16_t SELECT_DOWN_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_K};
static constexpr const uint16_t SELECT_DOWN_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_DOWN};
static constexpr const uint16_t SELECT_UP_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_L};
static constexpr const uint16_t SELECT_UP_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_UP};
static constexpr const uint16_t SELECT_RIGHT_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_SEMICOLON};
static constexpr const uint16_t SELECT_RIGHT_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_RIGHT};

static constexpr const uint16_t SELECT_TO_WORD_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_J};
static constexpr const uint16_t SELECT_TO_WORD_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_LEFT};
static constexpr const uint16_t SELECT_TO_WORD_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_SEMICOLON};
static constexpr const uint16_t SELECT_TO_WORD_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_RIGHT};

static constexpr const uint16_t SELECT_TO_LINE_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_H};
static constexpr const uint16_t SELECT_TO_LINE_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_HOME};
static constexpr const uint16_t SELECT_TO_LINE_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_APOSTROPHE};
static constexpr const uint16_t SELECT_TO_LINE_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_END};

static constexpr const uint16_t SELECT_PAGE_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_K};
static constexpr const uint16_t SELECT_PAGE_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_END};
static constexpr const uint16_t SELECT_PAGE_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_L};
static constexpr const uint16_t SELECT_PAGE_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_HOME};

static constexpr const Mapping MAPPINGS[] = {
	MAPPING(SELECT_ALL),
	MAPPING(CUT),
	MAPPING(COPY),
	MAPPING(PASTE),
	MAPPING(DELETE),
	MAPPING(FIND),
	MAPPING(GO_TO),
	MAPPING(OPEN),
	MAPPING(SAVE),
	MAPPING(PRINT),
	MAPPING(UNDO),
	MAPPING(REDO),
	MAPPING(OPEN_TAB),
	MAPPING(CLOSE_TAB),
	MAPPING(REOPEN_TAB),
	MAPPING(PLAY_PAUSE_SONG_ALT),
	MAPPING(PLAY_PAUSE_SONG_CTRL),
	MAPPING(PREVIOUS_SONG_ALT),
	MAPPING(PREVIOUS_SONG_CTRL),
	MAPPING(NEXT_SONG_ALT),
	MAPPING(NEXT_SONG_CTRL),
	MAPPING(VOLUME_UP_ALT),
	MAPPING(VOLUME_UP_CTRL),
	MAPPING(VOLUME_DOWN_ALT),
	MAPPING(VOLUME_DOWN_CTRL),
	MAPPING(MOVE_LEFT),
	MAPPING(MOVE_DOWN),
	MAPPING(MOVE_UP),
	MAPPING(MOVE_RIGHT),
	MAPPING(MOVE_TO_WORD_START),
	MAPPING(MOVE_TO_WORD_END),
	MAPPING(MOVE_TO_LINE_START),
	MAPPING(MOVE_TO_LINE_END),
	MAPPING(MOVE_PAGE_DOWN),
	MAPPING(MOVE_PAGE_UP),
	MAPPING(SELECT_LEFT),
	MAPPING(SELECT_DOWN),
	MAPPING(SELECT_UP),
	MAPPING(SELECT_RIGHT),
	MAPPING(SELECT_TO_WORD_START),
	MAPPING(SELECT_TO_WORD_END),
	MAPPING(SELECT_TO_LINE_START),
	MAPPING(SELECT_TO_LINE_END),
	MAPPING(SELECT_PAGE_END),
	MAPPING(SELECT_PAGE_START),
};


static bool running = true;


static void handle_input_event(const input_event &event, KeyboardState &state, const KeySpan *&previous_mapping, VirtualKeyboard &keyboard)
{
	if (event.type != EV_KEY or event.code >= KEY_COUNT)
		return;

#ifdef DEBUG
	switch (event.value) {
		case KeyStateRelease:
			fprintf(stderr, "\nInput:  Released %s\n", get_key_name(event.code));
			break;
		case KeyStatePress:
			fprintf(stderr, "\nInput:  Pressed %s\n", get_key_name(event.code));
			break;
		case KeyStateRepeat:
			fprintf(stderr, "\nInput:  Repeated %s\n", get_key_name(event.code));
			break;
		default:
			break;
	}
#endif

	// Remember the state
	switch (event.value) {
		case KeyStateRelease:
			state.clear(event.code);
			break;
		case KeyStatePress:
			state.set(event.code);
			break;
		case KeyStateRepeat:
			break;
		default:
			return;
	}

	input_event *output_event = output_events;
	size_t output_event_count = 0;

	// Remap
	constexpr const Mapping *MAPPING_END = MAPPINGS + sizeof(MAPPINGS) / sizeof(Mapping);
	for (const Mapping *mapping = MAPPINGS; mapping < MAPPING_END; mapping++) {
		// Skip if the mapping isn't needed
		if (mapping->from != state)
			continue;

		// If it's the same mapping, repeat the mapping and stop
		if (previous_mapping and *previous_mapping == mapping->to) {
			for (uint16_t key : mapping->to) {
			#ifdef DEBUG
				fprintf(stderr, "Output: Repeat previous map %s\n", get_key_name(key));
			#endif
				output_event->code = key;
				output_event->value = KeyStateRepeat;
				output_event += 2;
				output_event_count += 2;
			}
			keyboard.write(*output_events, output_event_count);
			return;
		}

		// Release the previous mapping
		if (previous_mapping) {
			for (uint16_t key : *previous_mapping) {
			#ifdef DEBUG
				fprintf(stderr, "Output: Release previous map %s\n", get_key_name(key));
			#endif
				output_event->code = key;
				output_event->value = KeyStateRelease;
				output_event += 2;
				output_event_count += 2;
			}
		}
		// Release the physically pressed keys
		else {
			// Current key
			if (event.value == KeyStateRelease) {
			#ifdef DEBUG
				fprintf(stderr, "Output: Release physically pressed %s\n", get_key_name(event.code));
			#endif
				output_event->code = event.code;
				output_event->value = KeyStateRelease;
				output_event += 2;
				output_event_count += 2;
			}
			// Other keys
			for (uint16_t key = 0; key < KEY_COUNT; key++) {
				if (not state.get(key) or key == event.code)
					continue;
			#ifdef DEBUG
				fprintf(stderr, "Output: Release physically pressed %s\n", get_key_name(key));
			#endif
				output_event->code = key;
				output_event->value = KeyStateRelease;
				output_event += 2;
				output_event_count += 2;
			}
		}

		// Remember this mapping
		previous_mapping = &mapping->to;

		// Press the "to" keys of the remap
		for (uint16_t key : mapping->to) {
		#ifdef DEBUG
			fprintf(stderr, "Output: Press map %s\n", get_key_name(key));
		#endif
			output_event->code = key;
			output_event->value = KeyStatePress;
			output_event += 2;
			output_event_count += 2;
		}

		// Stop after all of the changes
		keyboard.write(*output_events, output_event_count);
		return;
	}

	if (previous_mapping) {
		// Release the previous mapping
		for (uint16_t key : *previous_mapping) {
		#ifdef DEBUG
			fprintf(stderr, "Output: Release map %s\n", get_key_name(key));
		#endif
			output_event->code = key;
			output_event->value = KeyStateRelease;
			output_event += 2;
			output_event_count += 2;
		}
		// Press the physically pressed other than the new one, in reverse order
		// to press the modifier keys first
		for (uint16_t key = KEY_COUNT - 1; key-- > 0;) {
			if (not state.get(key) or key == event.code)
				continue;
		#ifdef DEBUG
			fprintf(stderr, "Output: Resume physically pressed %s\n", get_key_name(key));
		#endif
			output_event->code = key;
			output_event->value = KeyStatePress;
			output_event += 2;
			output_event_count += 2;
		}
	}
	previous_mapping = nullptr;

	// Press/release/repeat the same key
#ifdef DEBUG
	fprintf(stderr, "Output: Same key %s\n", get_key_name(event.code));
#endif
	output_event->code = event.code;
	output_event->value = event.value;
	output_event_count += 2;
	keyboard.write(*output_events, output_event_count);
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


int main(int argc, char **argv)
{
	// Parse arguments
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0)
			SUCCEED(APP_NAME ": keyboard remapper")
		if (strcmp(argv[i], "-v") == 0 or strcmp(argv[i], "--version") == 0)
			SUCCEED(APP_NAME " " APP_VERSION)
	}

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
	KeyboardState keyboard_states[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir)
			FAIL("Failed to open " PHYSICAL_DEVICE_DIRECTORY)

		char path[256] = PHYSICAL_DEVICE_DIRECTORY;

		// Each file
		uint8_t i = 0;
		for (const char *name = dir.read(); name and running; name = dir.read()) {
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

	input_event input_events[INPUT_EVENT_COUNT];

	const KeySpan *previous_mapping = nullptr;

	// Wait for the next general event
	while (running) {
		int event_count = epoll_wait(epoll_file, epoll_events, MAX_KEYBOARDS, -1);
		if (event_count < 0)
			continue;

		// Each general event
		for (int i = 0; i < event_count; i++) {
			epoll_event &general_event = epoll_events[i];
			int file = general_event.data.fd;

			// Get physical keyboard and state
			PhysicalKeyboard *physical = nullptr;
			KeyboardState *state = nullptr;
			for (size_t j = 0; j < MAX_KEYBOARDS; j++) {
				PhysicalKeyboard &current = physical_keyboards[j];
				if (current.file() == file) {
					physical = &current;
					state = &keyboard_states[j];
					break;
				}
			}
			if (not physical)
				continue;

			// Get input events
			int8_t input_event_count = physical->read(input_events);
			if (input_event_count < 0)
				continue;

			// Each input event
			for (size_t j = 0; j < static_cast<size_t>(input_event_count); j++)
				handle_input_event(input_events[j], *state, previous_mapping, virtual_keyboard);
		}
	}

	close(epoll_file);
}
