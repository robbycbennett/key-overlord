#pragma once


#include <stdint.h>


// Max keyboards total, where additional keyboards are ignored
constexpr uint8_t MAX_KEYBOARDS = 2;
// Max keyboards for a remapper, where additional keyboards are ignored
constexpr uint8_t MAX_KEYBOARDS_FOR_REMAPPER = 2;

// Directory of devices where the keyboard will be found (NOTE: end with a slash)
#define PHYSICAL_DEVICE_DIRECTORY "/dev/input/by-path/"
// End of file name of devices that are keyboards
#define PHYSICAL_DEVICE_NAME_END "0-event-kbd"
// Path to virtual device file, which you most likely won't change
#define VIRTUAL_DEVICE_PATH "/dev/uinput"


// Constraints
static_assert(MAX_KEYBOARDS >= MAX_KEYBOARDS_FOR_REMAPPER);
static_assert(MAX_KEYBOARDS > 0);
static_assert(MAX_KEYBOARDS_FOR_REMAPPER > 0);
static_assert(PHYSICAL_DEVICE_DIRECTORY[sizeof(PHYSICAL_DEVICE_DIRECTORY) - 2] == '/');
