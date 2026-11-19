#ifndef EDITOR_INFO_H
#define EDITOR_INFO_H

#if defined(_WIN32)
#include "../../vendor/raylib/windows/include/raylib.h"
#include "../../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../../vendor/raylib/unix/include/raylib.h"
#include "../../vendor/raylib/unix/include/raymath.h"
#endif

typedef struct {
	bool isPanning;
	bool mousePressed;
	bool mouseDown;
	bool mouseReleased;

	Vector2 mousePos;
	Vector2 prevPos;

	Vector2 origin;
	Vector2 scale;
	float rotation;

	int selectedLayer;
} editorState;

#endif
