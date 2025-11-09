#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <unistd.h>

#include "error.hpp"


int main(void)
{
	struct uinput_setup usetup;

	int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

	if (ioctl(fd, UI_SET_EVBIT, EV_KEY) == -1)
		FAIL("Failed ioctl 0")
	for (uint16_t key = 0; key < KEY_CNT; key++)
		if (ioctl(fd, UI_SET_KEYBIT, key) == -1)
			FAIL("Failed ioctl 1")

	memset(&usetup, 0, sizeof(usetup));
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1234;
	usetup.id.product = 0x5678;
	strcpy(usetup.name, "Example device");

	if (ioctl(fd, UI_DEV_SETUP, &usetup) == -1)
		FAIL("Failed ioctl 2")
	if (ioctl(fd, UI_DEV_CREATE) == -1)
		FAIL("Failed ioctl 3")

	// TODO instead of sleeping, wait until the device node is available, which
	// is what libevdev does in fetch_syspath_and_devnode
	constexpr __useconds_t MICROSECONDS = 1000 * 50;
	usleep(MICROSECONDS);

	struct input_event ie[4] = {
		{
			.time = {
				.tv_sec = 0,
				.tv_usec = 0,
			},
			.type = EV_KEY,
			.code = KEY_F,
			.value = 1,
		},
		{
			.time = {
				.tv_sec = 0,
				.tv_usec = 0,
			},
			.type = EV_SYN,
			.code = SYN_REPORT,
			.value = 0,
		},
		{
			.time = {
				.tv_sec = 0,
				.tv_usec = 0,
			},
			.type = EV_KEY,
			.code = KEY_F,
			.value = 0,
		},
		{
			.time = {
				.tv_sec = 0,
				.tv_usec = 0,
			},
			.type = EV_SYN,
			.code = SYN_REPORT,
			.value = 0,
		},
	};
	write(fd, &ie, sizeof(ie));

	if (ioctl(fd, UI_DEV_DESTROY) == -1)
		FAIL("Failed ioctl 4")
	close(fd);
}
