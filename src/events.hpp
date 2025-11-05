#pragma once


#include <stdint.h>

#include "keyboard_state.hpp"


static constexpr uint16_t EVENT_COUNT = KEY_COUNT * 2;


extern struct input_event events[EVENT_COUNT];
