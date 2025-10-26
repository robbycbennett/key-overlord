#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "physical_keyboard.hpp"


PhysicalKeyboard::PhysicalKeyboard(const char *path):
	m_file(open(path, O_RDWR))
{}


PhysicalKeyboard::~PhysicalKeyboard()
{
	if (m_file != -1)
		close(m_file);
}


bool PhysicalKeyboard::grab()
{
	return ioctl(m_file, EVIOCGRAB, 1) != -1;
}


bool PhysicalKeyboard::read(input_event &event)
{
	return ::read(m_file, &event, sizeof(input_event)) != -1;
}


bool PhysicalKeyboard::ungrab()
{
	return ioctl(m_file, EVIOCGRAB, 0) != -1;
}
