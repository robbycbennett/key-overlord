#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>

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

	if (ioctl(m_file, EVIOCGRAB, 1) == -1) {
		close();
		return false;
	}

	return true;
}


bool PhysicalKeyboard::read(input_event &event)
{
	return ::read(m_file, &event, sizeof(input_event)) != -1;
}


bool PhysicalKeyboard::write(const input_event &events, size_t event_count)
{
	return ::write(m_file, &events, sizeof(input_event) * event_count) != -1;
}


PhysicalKeyboard::operator bool() const
{
	return m_file != -1;
}
