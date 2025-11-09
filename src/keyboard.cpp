#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "keyboard.hpp"


Keyboard::~Keyboard()
{
	close();
}


bool Keyboard::close()
{
	if (m_file == -1)
		return true;
	if (m_virtual)
		ioctl(m_file, UI_DEV_DESTROY);
	else
		ungrab();
	int result = ::close(m_file);
	m_file = -1;
	return result == 0;
}


int Keyboard::file()
{
	return m_file;
}


bool Keyboard::grab()
{
	return ioctl(m_file, EVIOCGRAB, 1) != -1;
}


bool Keyboard::open_physical(const char *path)
{
	close();
	m_file = ::open(path, O_RDWR | O_NONBLOCK);
	m_virtual = false;
	return m_file != -1;
}


bool Keyboard::open_virtual()
{
	close();

	m_file = ::open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (m_file == -1)
		return false;
	m_virtual = false;

	if (ioctl(m_file, UI_SET_EVBIT, EV_KEY) == -1)
		return false;
	for (uint16_t key = 0; key < KEY_CNT; key++)
		if (ioctl(m_file, UI_SET_KEYBIT, key) == -1)
			return false;

	constexpr uinput_setup USETUP = uinput_setup {
		.id = input_id {
			.bustype = BUS_USB,
			.vendor = 0x1234,
			.product = 0x5678,
			.version = 0,
		},
		.name = "Key Overlord Virtual Keyboard",
		.ff_effects_max = 0,
	};

	if (ioctl(m_file, UI_DEV_SETUP, &USETUP) == -1)
		return false;
	if (ioctl(m_file, UI_DEV_CREATE) == -1)
		return false;

	m_virtual = true;
	return true;
}


bool Keyboard::read(input_event &event)
{
	return ::read(m_file, &event, sizeof(input_event)) != -1;
}


bool Keyboard::ungrab()
{
	return ioctl(m_file, EVIOCGRAB, 0) != -1;
}


bool Keyboard::write(const input_event &events, size_t event_count)
{
	return ::write(m_file, &events, sizeof(input_event) * event_count) != -1;
}


Keyboard::operator bool() const
{
	return m_file != -1;
}
