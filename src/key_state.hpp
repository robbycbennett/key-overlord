#pragma once


#include <stdint.h>


enum class KeyState: uint8_t {
	Release,
	Press,
	Repeat,
};
