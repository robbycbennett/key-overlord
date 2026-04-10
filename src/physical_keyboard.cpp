#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>

#include "key_state.hpp"
#include "output_events.hpp"
#include "physical_keyboard.hpp"


PhysicalKeyboard::~PhysicalKeyboard()
{
	close();
}


bool PhysicalKeyboard::close()
{
	if (m_file == -1)
		return true;

	bool success = true;
	if (ioctl(m_file, EVIOCGRAB, 0) == -1)
		success = false;
	if (::close(m_file) == -1)
		success = false;
	m_file = -1;
	return success;
}


int PhysicalKeyboard::file()
{
	return m_file;
}


bool PhysicalKeyboard::open(const char *path)
{
	close();

	m_file = ::open(path, O_RDWR | O_NONBLOCK);
	if (m_file == -1)
		return false;

	// Release all keys
	uint16_t key = 0;
	input_event *output_event = output_events;
	constexpr input_event *OUTPUT_EVENTS_END = output_events + EVENT_COUNT - 1;
	while (output_event < OUTPUT_EVENTS_END) {
		output_event->code = key;
		output_event->value = KeyStateRelease;
		output_event += 2;
		key += 1;
	}
	write(*output_events, EVENT_COUNT);

	if (ioctl(m_file, EVIOCGRAB, 1) == -1) {
		close();
		return false;
	}

	return true;
}


int8_t PhysicalKeyboard::read(input_event (&event)[INPUT_EVENT_COUNT])
{
	ssize_t bytes = ::read(m_file, &event, sizeof(input_event) * INPUT_EVENT_COUNT);
	if (bytes < 0)
		return -1;
	return static_cast<int8_t>(static_cast<size_t>(bytes) / sizeof(input_event));
}


bool PhysicalKeyboard::write(const input_event &events, size_t event_count)
{
	return ::write(m_file, &events, sizeof(input_event) * event_count) != -1;
}


PhysicalKeyboard::operator bool() const
{
	return m_file != -1;
}
