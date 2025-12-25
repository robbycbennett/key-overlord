#pragma once


#include "key_span.hpp"
#include "keyboard_state.hpp"


struct Mapping
{
	KeyboardState from;
	KeySpan to;
};
