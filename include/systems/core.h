#ifndef CORE_H
#define CORE_H

#include "../structs/canvas.h"
#include "../systems/editor_info.h"

typedef struct {
	Canvas canvas;
	editorState state;
} Core;

#endif
