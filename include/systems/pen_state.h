#ifndef PEN_STATE_H
#define PEN_STATE_H

#if defined(_WIN32)
#include "../../vendor/raylib/windows/include/raylib.h"
#include "../../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../../vendor/raylib/unix/include/raylib.h"
#include "../../vendor/raylib/unix/include/raymath.h"
#endif

typedef enum {
	PEN_NULL = 0,
	PEN_PRESSED         = 1u << 0,
	PEN_DOWN            = 1u << 1,
	PEN_RELEASED        = 1u << 2,
	MOUSE_LEFT_PRESSED  = 1u << 3,
	MOUSE_LEFT_DOWN     = 1u << 4,
	MOUSE_LEFT_RELEASED = 1u << 5,
	MOUSE_RIGHT_PRESSED  = 1u << 6,
	MOUSE_RIGHT_DOWN     = 1u << 7,
	MOUSE_RIGHT_RELEASED = 1u << 8,
} PEN_STATE;

#endif
