#pragma once


#include <stdint.h>


// Max keyboards total, where additional keyboards are ignored
constexpr uint8_t MAX_KEYBOARDS = 2;
// Max keyboards for a remapper, where additional keyboards are ignored
constexpr uint8_t MAX_KEYBOARDS_FOR_REMAPPER = 2;

// End of file name of devices that are keyboards
constexpr char PHYSICAL_DEVICE_NAME_END[] = "0-event-kbd";


// Constraints
static_assert(MAX_KEYBOARDS >= MAX_KEYBOARDS_FOR_REMAPPER);
static_assert(MAX_KEYBOARDS > 0);
static_assert(MAX_KEYBOARDS_FOR_REMAPPER > 0);
