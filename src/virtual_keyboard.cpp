#include <stdint.h>

#include <fcntl.h>
#include <linux/uinput.h>
#include <unistd.h>

#include "virtual_keyboard.hpp"


VirtualKeyboard::~VirtualKeyboard()
{
	close();
}


bool VirtualKeyboard::close()
{
	if (m_file == -1)
		return true;

	bool success = true;
	if (ioctl(m_file, UI_DEV_DESTROY) == -1)
		success = false;
	if (::close(m_file) == -1)
		success = false;
	m_file = -1;
	return success;
}


int VirtualKeyboard::file()
{
	return m_file;
}


bool VirtualKeyboard::open()
{
	close();

	m_file = ::open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (m_file == -1)
		return false;

	if (ioctl(m_file, UI_SET_EVBIT, EV_KEY) == -1) {
		close();
		return false;
	}
	for (uint16_t key = 0; key < KEY_CNT; key++) {
		if (ioctl(m_file, UI_SET_KEYBIT, key) == -1) {
			close();
			return false;
		}
	}

	constexpr uinput_setup USETUP = uinput_setup {
		.id = input_id {
			.bustype = BUS_USB,
			.vendor = 0x1234,
			.product = 0x5678,
			.version = 0,
		},
		.name = "Key Overlord Virtual VirtualKeyboard",
		.ff_effects_max = 0,
	};

	if (ioctl(m_file, UI_DEV_SETUP, &USETUP) == -1) {
		close();
		return false;
	}
	if (ioctl(m_file, UI_DEV_CREATE) == -1) {
		close();
		return false;
	}

	return true;
}


bool VirtualKeyboard::write(const input_event &events, size_t event_count)
{
	return ::write(m_file, &events, sizeof(input_event) * event_count) != -1;
}


VirtualKeyboard::operator bool() const
{
	return m_file != -1;
}
