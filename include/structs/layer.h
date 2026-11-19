#pragma once
#ifndef LAYER_H
#define LAYER_H

#if defined(_WIN32)
#include "../../vendor/raylib/windows/include/raylib.h"
#include "../../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../../vendor/raylib/unix/include/raylib.h"
#include "../../vendor/raylib/unix/include/raymath.h"
#endif

typedef struct {
    RenderTexture2D tex;
	BlendMode blending_mode;
	char opacity;
	// other stuff
} Layer;

Layer createLayer(Vector2 dimensions);

#endif
