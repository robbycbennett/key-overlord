#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "physical_keyboard.hpp"


PhysicalKeyboard::~PhysicalKeyboard()
{
	close();
}


bool PhysicalKeyboard::close()
{
	if (m_file == -1)
		return false;
	int result = ::close(m_file);
	m_file = -1;
	return result == 0;
}


bool PhysicalKeyboard::grab()
{
	return ioctl(m_file, EVIOCGRAB, 1) != -1;
}


bool PhysicalKeyboard::open(const char *path)
{
	close();
	m_file = ::open(path, O_RDWR);
	return m_file != -1;
}


bool PhysicalKeyboard::read(input_event &event)
{
	return ::read(m_file, &event, sizeof(input_event)) != -1;
}


bool PhysicalKeyboard::ungrab()
{
	return ioctl(m_file, EVIOCGRAB, 0) != -1;
}
