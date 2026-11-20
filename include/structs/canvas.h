#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#if defined(_WIN32)
#include "../../vendor/raylib/windows/include/raylib.h"
#include "../../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../../vendor/raylib/unix/include/raylib.h"
#include "../../vendor/raylib/unix/include/raymath.h"
#endif

#include "./layer.h"
#include "../systems/editor_info.h"
#include <stddef.h>

typedef struct {
	// other stuff
	Vector2 dimensions;
	Layer* layers;      // heap allocated array, topmost layer is first in count
	size_t layer_count; 
} Canvas;

void create_layer(Canvas* canvas, const char* layer_name);
#endif
