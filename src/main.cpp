#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <linux/input-event-codes.h>
#include <linux/limits.h>
#include <linux/uinput.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <unistd.h>

#include "config.hpp"
#include "dir.hpp"
#include "dir_iterator.hpp"
#include "key_name.hpp"
#include "key_stack.hpp"
#include "key_state.hpp"
#include "keyboard_state.hpp"
#include "mapping.hpp"
#include "name.hpp"
#include "output_events.hpp"
#include "physical_keyboard.hpp"
#include "virtual_keyboard.hpp"


#define APP_NAME "keyoverlord"
#define APP_VERSION "1.0.0"


enum class AcquireKeyboardResult: uint8_t
{
	Ok,
	NotAPhysicalDevice,
	PathTooLarge,
	UnableToOpenKeyboard,
	UnableToWatchKeyboard,
};


static bool running = true;


// Open a keyboard for reading and writing and listen to it
static AcquireKeyboardResult acquire_keyboard(
	const char &name,
	size_t name_length,
	PhysicalKeyboard &keyboard,
	int epoll_file,
	epoll_event &event);

// Remember the keyboard state and write to the virtual keyboard
static void handle_input_event(
	const input_event &event,
	KeyboardState &state,
	KeyStack &stack,
	const KeySpan *&previous_mapping_ptr,
	VirtualKeyboard &keyboard);

// Set the global variable `running` to false
static void handle_signal(int signal_number);

// Whether the string ends in "0-event-kbd"
static bool is_physical_device(const char &string, size_t length);


int main(int argc, char **argv)
{
	// TODO fix bugs with starting/stopping with the remapped keyboard, possibly related to stdin, stdout, stderr
	constexpr const char PHYSICAL_DEVICE_DIRECTORY[] = "/dev/input/by-path";

	// Parse arguments
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 or strcmp(argv[i], "--help") == 0) {
			fputs(APP_NAME ": keyboard remapper\n", stdout);
			return 0;
		}
		if (strcmp(argv[i], "-v") == 0 or strcmp(argv[i], "--version") == 0) {
			fputs(APP_NAME " " APP_VERSION "\n", stdout);
			return 0;
		}
	}

	// Fail if not root
	if (getuid() != 0) {
		fputs(APP_NAME ": Run as root to acquire keyboards\n", stderr);
		return 1;
	}

	// Handle signals gracefully
	signal(SIGHUP,  handle_signal);
	signal(SIGINT,  handle_signal);
	signal(SIGQUIT, handle_signal);
	signal(SIGABRT, handle_signal);
	signal(SIGTERM, handle_signal);

	// Acquire the virtual keyboard
	VirtualKeyboard virtual_keyboard;
	if (not virtual_keyboard.open()) {
		fputs(APP_NAME ": Failed to create a virtual keyboard\n", stderr);
		return 1;
	}

	// Create a queue to listen to filesystem events
	int inotify_file = inotify_init();
	if (inotify_file == -1) {
		fputs(APP_NAME ": Failed to create a queue to watch the filesystem\n", stderr);
		return 1;
	}

	// Listen to changes of the directory
	constexpr uint32_t DIRECTORY_EVENT_KINDS = IN_MOVED_TO | IN_DELETE;
	if (inotify_add_watch(inotify_file, PHYSICAL_DEVICE_DIRECTORY, DIRECTORY_EVENT_KINDS) == -1) {
		fputs(APP_NAME ": Failed to create watch the directory /dev/input/by-path\n", stderr);
		return 1;
	}

	// Prepare to listen to keyboard events
	int epoll_file = epoll_create1(0);
	if (epoll_file == -1) {
		fputs(APP_NAME ": Failed to create an event poll file\n", stderr);
		return 1;
	}

	// Configure what events to listen to
	constexpr uint8_t MAX_EPOLL_EVENTS = MAX_KEYBOARDS + 1;
	static_assert(MAX_EPOLL_EVENTS > 0);
	static_assert(MAX_EPOLL_EVENTS > MAX_KEYBOARDS);
	epoll_event epoll_events[MAX_EPOLL_EVENTS] = {
		epoll_event {
			.events = EPOLLIN,
			.data = epoll_data { .fd = -1 },
		},
		epoll_event {
			.events = EPOLLIN,
			.data = epoll_data { .fd = -1 },
		},
		epoll_event {
			.events = EPOLLIN,
			.data = epoll_data { .fd = inotify_file },
		},
	};

	// Listen to the filesystem change queue
	epoll_event *epoll_event_inotify = epoll_events + MAX_KEYBOARDS;
	if (epoll_ctl(epoll_file, EPOLL_CTL_ADD, inotify_file, epoll_event_inotify) == -1) {
		fputs(APP_NAME ": Failed to watch the filesystem change queue\n", stderr);
		return 1;
	}

	// Acquire physical keyboards and listen to them
	PhysicalKeyboard physical_keyboards[MAX_KEYBOARDS];
	KeyboardState keyboard_states[MAX_KEYBOARDS];
	KeyStack key_stacks[MAX_KEYBOARDS];
	Name keyboard_names[MAX_KEYBOARDS];
	{
		Dir dir(PHYSICAL_DEVICE_DIRECTORY);
		if (not dir) {
			fputs(APP_NAME ": Failed to open the directory /dev/input/by-path\n", stderr);
			return 1;
		}

		// Each file
		PhysicalKeyboard *keyboard = physical_keyboards;
		PhysicalKeyboard *end = physical_keyboards + MAX_KEYBOARDS;
		epoll_event *event = epoll_events;
		Name *saved_name = keyboard_names;

		uint8_t dir_entry_buffer[1024];
		DirIterator dir_iterator(dir, *dir_entry_buffer, sizeof(dir_entry_buffer));
		for (const char *name = dir_iterator.next_name(); name; name = dir_iterator.next_name()) {
			size_t name_length = strlen(name);
			if (name_length > NAME_MAX)
				continue;
			switch (acquire_keyboard(*name, name_length, *keyboard, epoll_file, *event)) {
				using enum AcquireKeyboardResult;
				case Ok:
				#ifdef DEBUG
					fprintf(stderr, "\nAdd:    %s\n", name);
				#endif
					saved_name->set(*name, name_length);
					keyboard++;
					event++;
					saved_name++;
					if (keyboard >= end)
						goto acquire_loop_end;
					continue;
				case PathTooLarge:
					fputs("Path of physical keyboard device is too large\n", stderr);
					continue;
				case UnableToWatchKeyboard:
					fputs("Failed to watch a physical keyboard device\n", stderr);
					continue;
				case NotAPhysicalDevice:
				case UnableToOpenKeyboard:
				default:
					continue;
			}
		}
	}
acquire_loop_end:

	input_event input_events[INPUT_EVENT_COUNT];

	const KeySpan *previous_mapping_ptr = nullptr;

	// Wait for the next general event
	while (running) {
		int event_count = epoll_wait(epoll_file, epoll_events, MAX_KEYBOARDS, -1);
		if (event_count <= 0)
			continue;

		// Each general event
		epoll_event *event_end = epoll_events + event_count;
		for (epoll_event *event = epoll_events; event < event_end; event++) {
			int file = event->data.fd;

			// Handle a plugged/unplugged keyboard
			if (file == inotify_file) {
				struct INotifyEvent
				{
					int32_t wd;
					uint32_t mask;
					uint32_t cookie;
					uint32_t len;
					char name[NAME_MAX + 1];
				};
				static_assert(offsetof(INotifyEvent, wd) == offsetof(inotify_event, wd));
				static_assert(offsetof(INotifyEvent, mask) == offsetof(inotify_event, mask));
				static_assert(offsetof(INotifyEvent, cookie) == offsetof(inotify_event, cookie));
				static_assert(offsetof(INotifyEvent, len) == offsetof(inotify_event, len));
				static_assert(offsetof(INotifyEvent, name) == offsetof(inotify_event, name));

				INotifyEvent dir_event;

				// Read or try again later
				ssize_t bytes_read = read(inotify_file, &dir_event, sizeof(dir_event));
				if (bytes_read <= 0)
					continue;

				char &name = *dir_event.name;
				size_t name_length = strlen(&name);

				// Plugged in a keyboard
				if (dir_event.mask & IN_MOVED_TO) {
					// Find an available keyboard and epoll event pair
					PhysicalKeyboard *keyboard = nullptr;
					Name *saved_name = keyboard_names;
					for (PhysicalKeyboard *other = physical_keyboards;
							other < physical_keyboards + MAX_KEYBOARDS;
							other++, saved_name++) {
						if (other->file() == -1) {
							keyboard = other;
							break;
						}
					}
					if (not keyboard)
						continue;

					switch (acquire_keyboard(name, name_length, *keyboard, epoll_file, *event)) {
						using enum AcquireKeyboardResult;
						case Ok:
						#ifdef DEBUG
							fprintf(stderr, "\nAdd:    %s\n", &name);
						#endif
							saved_name->set(name, name_length);
							continue;
						case PathTooLarge:
							fputs("Path of physical keyboard device is too large\n", stderr);
							continue;
						case UnableToWatchKeyboard:
							fputs("Failed to watch a physical keyboard device\n", stderr);
							continue;
						case NotAPhysicalDevice:
						case UnableToOpenKeyboard:
						default:
							continue;
					}
				}
				// Unplugged a keyboard
				else if (dir_event.mask & IN_DELETE) {
					if (not is_physical_device(name, name_length))
						continue;

					// Find the keyboard and epoll event by the name
					PhysicalKeyboard *keyboard = nullptr;
					epoll_event *event_for_listening = epoll_events;
					Name *saved_name = keyboard_names;
					for (PhysicalKeyboard *other = physical_keyboards;
							other < physical_keyboards + MAX_KEYBOARDS;
							other++, event_for_listening++, saved_name++) {
						if (other->file() == -1)
							continue;
						if (not saved_name->equals(name, name_length))
							continue;
						keyboard = other;
						break;
					}

					if (not keyboard)
						continue;

				#ifdef DEBUG
					fprintf(stderr, "\nRemove: %s\n", dir_event.name);
				#endif

					saved_name->clear();

					// Stop watching the keyboard and forget its file descriptor
					if (epoll_ctl(epoll_file, EPOLL_CTL_DEL, keyboard->file(), event) == -1)
						fputs("Failed to stop watching the keyboard\n", stderr);
					event->data.fd = -1;

					// Close the keyboard file
					keyboard->close();

				}
				continue;
			}

			// Get physical keyboard and state
			PhysicalKeyboard *physical = nullptr;
			KeyboardState *state = nullptr;
			KeyStack *stack = nullptr;
			for (size_t i = 0; i < MAX_KEYBOARDS; i++) {
				PhysicalKeyboard &current = physical_keyboards[i];
				if (current.file() == file) {
					physical = &current;
					state = &keyboard_states[i];
					stack = &key_stacks[i];
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
			input_event *end = input_events + input_event_count;
			for (input_event *i = input_events; i < end; i++)
				handle_input_event(*i, *state, *stack, previous_mapping_ptr, virtual_keyboard);
		}
	}

	close(epoll_file);
}


static AcquireKeyboardResult acquire_keyboard(
	const char &name,
	size_t name_length,
	PhysicalKeyboard &keyboard,
	int epoll_file,
	epoll_event &event)
{
	using enum AcquireKeyboardResult;

	// File path of keyboard
	char path[256] = "/dev/input/by-path/";
	constexpr size_t PATH_PREFIX = 20;
	static_assert(PATH_PREFIX < sizeof(path));

	// Skip everything but physical keyboards
	if (not is_physical_device(name, name_length))
		return NotAPhysicalDevice;

	// Get the path to the keyboard
	if (PATH_PREFIX + name_length + 1 > sizeof(path))
		return PathTooLarge;
	memcpy(path + PATH_PREFIX - 1, &name, name_length + 1);

	// Open for reading and writing
	if (not keyboard.open(path))
		return UnableToOpenKeyboard;

	// Watch the keyboard and remember the file descriptor
	event.data.fd = keyboard.file();
	if (epoll_ctl(epoll_file, EPOLL_CTL_ADD, keyboard.file(), &event) == -1) {
		event.data.fd = -1;
		keyboard.close();
		return UnableToWatchKeyboard;
	}

	return Ok;
}


static void handle_input_event(
	const input_event &event,
	KeyboardState &state,
	KeyStack &stack,
	const KeySpan *&previous_mapping_ptr,
	VirtualKeyboard &keyboard)
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
			stack.remove(event.code);
			break;
		case KeyStatePress:
			state.set(event.code);
			stack.push(event.code);
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

		KeySpan mapping_to = mapping->to;

		if (previous_mapping_ptr) {
			KeySpan previous_mapping = *previous_mapping_ptr;

			// If it's the same mapping, repeat the last key and stop
			if (previous_mapping == mapping_to) {
				if (mapping_to.is_empty()) {
					return;
				}
				uint16_t key = mapping_to.last();
			#ifdef DEBUG
				fprintf(stderr, "Output: Repeat previous map %s\n", get_key_name(key));
			#endif
				output_event->code = key;
				output_event->value = KeyStateRepeat;
				output_event += 2;
				output_event_count += 2;
				keyboard.write(*output_events, output_event_count);
				return;
			}

			// Release the previous mapping
			for (uint16_t key : previous_mapping.reverse_range()) {
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
			for (uint16_t key : stack.reverse_range()) {
				if (key == event.code) {
					continue;
				}
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
		previous_mapping_ptr = &mapping->to;

		// Press the "to" keys of the remap
		for (uint16_t key : mapping_to) {
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

	if (previous_mapping_ptr) {
		// Release the previous mapping
		for (uint16_t key : previous_mapping_ptr->reverse_range()) {
		#ifdef DEBUG
			fprintf(stderr, "Output: Release previous map %s\n", get_key_name(key));
		#endif
			output_event->code = key;
			output_event->value = KeyStateRelease;
			output_event += 2;
			output_event_count += 2;
		}

		// Press the physically pressed other than the new one
		if (event.value == KeyStatePress) {
			for (uint16_t key : stack) {
				if (key == event.code) {
					continue;
				}
			#ifdef DEBUG
				fprintf(stderr, "Output: Resume physically pressed %s\n", get_key_name(key));
			#endif
				output_event->code = key;
				output_event->value = KeyStatePress;
				output_event += 2;
				output_event_count += 2;
			}
		}

		previous_mapping_ptr = nullptr;
	}

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


static bool is_physical_device(const char &string, size_t length)
{
	constexpr size_t SUBSTR_LENGTH = sizeof(PHYSICAL_DEVICE_NAME_END) - 1;
	if (length < SUBSTR_LENGTH)
		return false;
	return memcmp(&string + length - SUBSTR_LENGTH, PHYSICAL_DEVICE_NAME_END, SUBSTR_LENGTH) == 0;
}
