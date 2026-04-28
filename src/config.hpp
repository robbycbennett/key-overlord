#pragma once


#include <stdint.h>

#include <linux/input-event-codes.h>

#include "mapping.hpp"


// Max keyboards total, where additional keyboards are ignored
constexpr uint8_t MAX_KEYBOARDS = 2;
static_assert(MAX_KEYBOARDS > 0);

// End of file name of devices that are keyboards
constexpr char PHYSICAL_DEVICE_NAME_END[] = "0-event-kbd";


#define MAPPING(array) \
	Mapping( \
		KeySpan {array##_INPUT, sizeof(array##_INPUT) / sizeof(uint16_t)}, \
		KeySpan {array##_OUTPUT, sizeof(array##_OUTPUT) / sizeof(uint16_t)} \
	)

#define MAPPING_BOTH(array) \
	Mapping( \
		KeySpan {array##_INPUT_1, sizeof(array##_INPUT_1) / sizeof(uint16_t)}, \
		KeySpan {array##_OUTPUT, sizeof(array##_OUTPUT) / sizeof(uint16_t)} \
	), \
	Mapping( \
		KeySpan {array##_INPUT_2, sizeof(array##_INPUT_2) / sizeof(uint16_t)}, \
		KeySpan {array##_OUTPUT, sizeof(array##_OUTPUT) / sizeof(uint16_t)} \
	)

static constexpr const uint16_t SELECT_ALL_INPUT_1[] =
	{KEY_LEFTALT, KEY_A};
static constexpr const uint16_t SELECT_ALL_INPUT_2[] =
	{KEY_RIGHTALT, KEY_A};
static constexpr const uint16_t SELECT_ALL_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_A};

static constexpr const uint16_t CUT_INPUT_1[] =
	{KEY_LEFTALT, KEY_X};
static constexpr const uint16_t CUT_INPUT_2[] =
	{KEY_RIGHTALT, KEY_X};
static constexpr const uint16_t CUT_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_X};
static constexpr const uint16_t COPY_INPUT_1[] =
	{KEY_LEFTALT, KEY_C};
static constexpr const uint16_t COPY_INPUT_2[] =
	{KEY_RIGHTALT, KEY_C};
static constexpr const uint16_t COPY_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_C};
static constexpr const uint16_t PASTE_INPUT_1[] =
	{KEY_LEFTALT, KEY_V};
static constexpr const uint16_t PASTE_INPUT_2[] =
	{KEY_RIGHTALT, KEY_V};
static constexpr const uint16_t PASTE_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_V};

static constexpr const uint16_t DELETE_INPUT_1[] =
	{KEY_LEFTALT, KEY_D};
static constexpr const uint16_t DELETE_INPUT_2[] =
	{KEY_RIGHTALT, KEY_D};
static constexpr const uint16_t DELETE_OUTPUT[] =
	{KEY_DELETE};

static constexpr const uint16_t FIND_INPUT_1[] =
	{KEY_LEFTALT, KEY_F};
static constexpr const uint16_t FIND_INPUT_2[] =
	{KEY_RIGHTALT, KEY_F};
static constexpr const uint16_t FIND_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_F};
static constexpr const uint16_t GO_TO_INPUT_1[] =
	{KEY_LEFTALT, KEY_G};
static constexpr const uint16_t GO_TO_INPUT_2[] =
	{KEY_RIGHTALT, KEY_G};
static constexpr const uint16_t GO_TO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_G};

static constexpr const uint16_t OPEN_INPUT_1[] =
	{KEY_LEFTALT, KEY_O};
static constexpr const uint16_t OPEN_INPUT_2[] =
	{KEY_RIGHTALT, KEY_O};
static constexpr const uint16_t OPEN_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_O};
static constexpr const uint16_t SAVE_INPUT_1[] =
	{KEY_LEFTALT, KEY_S};
static constexpr const uint16_t SAVE_INPUT_2[] =
	{KEY_RIGHTALT, KEY_S};
static constexpr const uint16_t SAVE_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_S};
static constexpr const uint16_t PRINT_INPUT_1[] =
	{KEY_LEFTALT, KEY_P};
static constexpr const uint16_t PRINT_INPUT_2[] =
	{KEY_RIGHTALT, KEY_P};
static constexpr const uint16_t PRINT_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_P};

static constexpr const uint16_t UNDO_INPUT_1[] =
	{KEY_LEFTALT, KEY_U};
static constexpr const uint16_t UNDO_INPUT_2[] =
	{KEY_RIGHTALT, KEY_U};
static constexpr const uint16_t UNDO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_Z};
static constexpr const uint16_t REDO_INPUT_1[] =
	{KEY_LEFTALT, KEY_I};
static constexpr const uint16_t REDO_INPUT_2[] =
	{KEY_RIGHTALT, KEY_I};
static constexpr const uint16_t REDO_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_Y};

static constexpr const uint16_t OPEN_TAB_INPUT_1[] =
	{KEY_LEFTALT, KEY_N};
static constexpr const uint16_t OPEN_TAB_INPUT_2[] =
	{KEY_RIGHTALT, KEY_N};
static constexpr const uint16_t OPEN_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_T};
static constexpr const uint16_t CLOSE_TAB_INPUT_1[] =
	{KEY_LEFTALT, KEY_M};
static constexpr const uint16_t CLOSE_TAB_INPUT_2[] =
	{KEY_RIGHTALT, KEY_M};
static constexpr const uint16_t CLOSE_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_W};
static constexpr const uint16_t REOPEN_TAB_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_N};
static constexpr const uint16_t REOPEN_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTSHIFT, KEY_T};
static constexpr const uint16_t PREVIOUS_TAB_INPUT_1[] =
	{KEY_LEFTALT, KEY_COMMA};
static constexpr const uint16_t PREVIOUS_TAB_INPUT_2[] =
	{KEY_RIGHTALT, KEY_COMMA};
static constexpr const uint16_t PREVIOUS_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTSHIFT, KEY_TAB};
static constexpr const uint16_t NEXT_TAB_INPUT_1[] =
	{KEY_LEFTALT, KEY_DOT};
static constexpr const uint16_t NEXT_TAB_INPUT_2[] =
	{KEY_RIGHTALT, KEY_DOT};
static constexpr const uint16_t NEXT_TAB_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_TAB};

static constexpr const uint16_t PLAY_PAUSE_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_SPACE};
static constexpr const uint16_t PLAY_PAUSE_SONG_ALT_OUTPUT[] =
	{KEY_PLAYPAUSE};
static constexpr const uint16_t PLAY_PAUSE_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_SPACE};
static constexpr const uint16_t PLAY_PAUSE_SONG_CTRL_OUTPUT[] =
	{KEY_PLAYPAUSE};
static constexpr const uint16_t PREVIOUS_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_LEFT};
static constexpr const uint16_t PREVIOUS_SONG_ALT_OUTPUT[] =
	{KEY_PREVIOUSSONG};
static constexpr const uint16_t PREVIOUS_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_LEFT};
static constexpr const uint16_t PREVIOUS_SONG_CTRL_OUTPUT[] =
	{KEY_PREVIOUSSONG};
static constexpr const uint16_t NEXT_SONG_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_RIGHT};
static constexpr const uint16_t NEXT_SONG_ALT_OUTPUT[] =
	{KEY_NEXTSONG};
static constexpr const uint16_t NEXT_SONG_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_RIGHT};
static constexpr const uint16_t NEXT_SONG_CTRL_OUTPUT[] =
	{KEY_NEXTSONG};
static constexpr const uint16_t VOLUME_UP_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_UP};
static constexpr const uint16_t VOLUME_UP_ALT_OUTPUT[] =
	{KEY_VOLUMEUP};
static constexpr const uint16_t VOLUME_UP_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_UP};
static constexpr const uint16_t VOLUME_UP_CTRL_OUTPUT[] =
	{KEY_VOLUMEUP};
static constexpr const uint16_t VOLUME_DOWN_ALT_INPUT[] =
	{KEY_RIGHTALT, KEY_DOWN};
static constexpr const uint16_t VOLUME_DOWN_ALT_OUTPUT[] =
	{KEY_VOLUMEDOWN};
static constexpr const uint16_t VOLUME_DOWN_CTRL_INPUT[] =
	{KEY_RIGHTCTRL, KEY_DOWN};
static constexpr const uint16_t VOLUME_DOWN_CTRL_OUTPUT[] =
	{KEY_VOLUMEDOWN};

static constexpr const uint16_t MOVE_LEFT_INPUT_1[] =
	{KEY_LEFTALT, KEY_J};
static constexpr const uint16_t MOVE_LEFT_INPUT_2[] =
	{KEY_RIGHTALT, KEY_J};
static constexpr const uint16_t MOVE_LEFT_OUTPUT[] =
	{KEY_LEFT};
static constexpr const uint16_t MOVE_DOWN_INPUT_1[] =
	{KEY_LEFTALT, KEY_K};
static constexpr const uint16_t MOVE_DOWN_INPUT_2[] =
	{KEY_RIGHTALT, KEY_K};
static constexpr const uint16_t MOVE_DOWN_OUTPUT[] =
	{KEY_DOWN};
static constexpr const uint16_t MOVE_UP_INPUT_1[] =
	{KEY_LEFTALT, KEY_L};
static constexpr const uint16_t MOVE_UP_INPUT_2[] =
	{KEY_RIGHTALT, KEY_L};
static constexpr const uint16_t MOVE_UP_OUTPUT[] =
	{KEY_UP};
static constexpr const uint16_t MOVE_RIGHT_INPUT_1[] =
	{KEY_LEFTALT, KEY_SEMICOLON};
static constexpr const uint16_t MOVE_RIGHT_INPUT_2[] =
	{KEY_RIGHTALT, KEY_SEMICOLON};
static constexpr const uint16_t MOVE_RIGHT_OUTPUT[] =
	{KEY_RIGHT};

static constexpr const uint16_t MOVE_TO_WORD_START_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_J};
static constexpr const uint16_t MOVE_TO_WORD_START_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_LEFT};
static constexpr const uint16_t MOVE_TO_WORD_END_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_SEMICOLON};
static constexpr const uint16_t MOVE_TO_WORD_END_OUTPUT[] =
	{KEY_LEFTCTRL, KEY_RIGHT};

static constexpr const uint16_t MOVE_TO_LINE_START_INPUT_1[] =
	{KEY_LEFTALT, KEY_H};
static constexpr const uint16_t MOVE_TO_LINE_START_INPUT_2[] =
	{KEY_RIGHTALT, KEY_H};
static constexpr const uint16_t MOVE_TO_LINE_START_OUTPUT[] =
	{KEY_HOME};
static constexpr const uint16_t MOVE_TO_LINE_END_INPUT_1[] =
	{KEY_LEFTALT, KEY_APOSTROPHE};
static constexpr const uint16_t MOVE_TO_LINE_END_INPUT_2[] =
	{KEY_RIGHTALT, KEY_APOSTROPHE};
static constexpr const uint16_t MOVE_TO_LINE_END_OUTPUT[] =
	{KEY_END};

static constexpr const uint16_t MOVE_PAGE_DOWN_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_K};
static constexpr const uint16_t MOVE_PAGE_DOWN_OUTPUT[] =
	{KEY_PAGEDOWN};
static constexpr const uint16_t MOVE_PAGE_UP_INPUT[] =
	{KEY_LEFTALT, KEY_LEFTSHIFT, KEY_L};
static constexpr const uint16_t MOVE_PAGE_UP_OUTPUT[] =
	{KEY_PAGEUP};

static constexpr const uint16_t SELECT_LEFT_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_J};
static constexpr const uint16_t SELECT_LEFT_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFT};
static constexpr const uint16_t SELECT_DOWN_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_K};
static constexpr const uint16_t SELECT_DOWN_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_DOWN};
static constexpr const uint16_t SELECT_UP_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_L};
static constexpr const uint16_t SELECT_UP_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_UP};
static constexpr const uint16_t SELECT_RIGHT_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_SEMICOLON};
static constexpr const uint16_t SELECT_RIGHT_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_RIGHT};

static constexpr const uint16_t SELECT_TO_WORD_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_J};
static constexpr const uint16_t SELECT_TO_WORD_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_LEFT};
static constexpr const uint16_t SELECT_TO_WORD_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_SEMICOLON};
static constexpr const uint16_t SELECT_TO_WORD_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_RIGHT};

static constexpr const uint16_t SELECT_TO_LINE_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_H};
static constexpr const uint16_t SELECT_TO_LINE_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_HOME};
static constexpr const uint16_t SELECT_TO_LINE_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_APOSTROPHE};
static constexpr const uint16_t SELECT_TO_LINE_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_END};

static constexpr const uint16_t SELECT_PAGE_END_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_K};
static constexpr const uint16_t SELECT_PAGE_END_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_END};
static constexpr const uint16_t SELECT_PAGE_START_INPUT[] =
	{KEY_LEFTCTRL, KEY_LEFTALT, KEY_LEFTSHIFT, KEY_L};
static constexpr const uint16_t SELECT_PAGE_START_OUTPUT[] =
	{KEY_LEFTSHIFT, KEY_LEFTCTRL, KEY_HOME};

static constexpr const Mapping MAPPINGS[] = {
	MAPPING_BOTH(SELECT_ALL),
	MAPPING_BOTH(CUT),
	MAPPING_BOTH(COPY),
	MAPPING_BOTH(PASTE),
	MAPPING_BOTH(DELETE),
	MAPPING_BOTH(FIND),
	MAPPING_BOTH(GO_TO),
	MAPPING_BOTH(OPEN),
	MAPPING_BOTH(SAVE),
	MAPPING_BOTH(PRINT),
	MAPPING_BOTH(UNDO),
	MAPPING_BOTH(REDO),
	MAPPING_BOTH(OPEN_TAB),
	MAPPING_BOTH(CLOSE_TAB),
	MAPPING(REOPEN_TAB),
	MAPPING_BOTH(PREVIOUS_TAB),
	MAPPING_BOTH(NEXT_TAB),
	MAPPING(PLAY_PAUSE_SONG_ALT),
	MAPPING(PLAY_PAUSE_SONG_CTRL),
	MAPPING(PREVIOUS_SONG_ALT),
	MAPPING(PREVIOUS_SONG_CTRL),
	MAPPING(NEXT_SONG_ALT),
	MAPPING(NEXT_SONG_CTRL),
	MAPPING(VOLUME_UP_ALT),
	MAPPING(VOLUME_UP_CTRL),
	MAPPING(VOLUME_DOWN_ALT),
	MAPPING(VOLUME_DOWN_CTRL),
	MAPPING_BOTH(MOVE_LEFT),
	MAPPING_BOTH(MOVE_DOWN),
	MAPPING_BOTH(MOVE_UP),
	MAPPING_BOTH(MOVE_RIGHT),
	MAPPING(MOVE_TO_WORD_START),
	MAPPING(MOVE_TO_WORD_END),
	MAPPING_BOTH(MOVE_TO_LINE_START),
	MAPPING_BOTH(MOVE_TO_LINE_END),
	MAPPING(MOVE_PAGE_DOWN),
	MAPPING(MOVE_PAGE_UP),
	MAPPING(SELECT_LEFT),
	MAPPING(SELECT_DOWN),
	MAPPING(SELECT_UP),
	MAPPING(SELECT_RIGHT),
	MAPPING(SELECT_TO_WORD_START),
	MAPPING(SELECT_TO_WORD_END),
	MAPPING(SELECT_TO_LINE_START),
	MAPPING(SELECT_TO_LINE_END),
	MAPPING(SELECT_PAGE_END),
	MAPPING(SELECT_PAGE_START),
};

#undef MAPPING
