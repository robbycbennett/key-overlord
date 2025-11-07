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
	return m_file != -1;
}


bool Keyboard::open_virtual()
{
	close();
	m_file = ::open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (m_file == -1)
		return false;

	uinput_setup usetup;
	if (ioctl(m_file, UI_SET_EVBIT, EV_KEY) == -1)
		return false;

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1234;
	usetup.id.product = 0x5678;
	strcpy(usetup.name, "Key Overlord Virtual Keyboard");

	if (ioctl(m_file, UI_DEV_SETUP, &usetup) == -1)
		return false;
	if (ioctl(m_file, UI_DEV_CREATE) == -1)
		return false;

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
