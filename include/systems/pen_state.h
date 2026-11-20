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
	PEN_PRESSED,
	PEN_DOWN,
	PEN_RELEASED,
} PEN_STATE;

#endif
