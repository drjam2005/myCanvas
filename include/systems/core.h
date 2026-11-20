#ifndef CORE_H
#define CORE_H

#include "../structs/canvas.h"
#include "../systems/shortcut.h"
#include "../systems/editor_info.h"

typedef struct Core {
	Canvas canvas;
	editorState state;

	ShortcutManager shortcuts;
} Core;

#endif
