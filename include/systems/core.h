#ifndef CORE_H
#define CORE_H

#include "../structs/canvas.h"
#include "../systems/shortcut.h"
#include "../systems/editor_info.h"

#define MAX_UNDO_HISTORY 10

typedef struct {
	char layer_name[64];
    Image img;
	BlendMode blending_mode;
	char opacity;
} LayerSnapshot;

typedef struct {				// for undo history
	LayerSnapshot* layers;      // heap allocated array, topmost layer is first in count
	size_t layer_count;
	int selected_layer;
} CanvasSnapshot;

typedef struct Core {
	Canvas canvas;
	CanvasSnapshot snapshots[MAX_UNDO_HISTORY];
	size_t snapshot_count;
	size_t snapshot_index; // 0 is the newest snapshot
	bool stroke_changed;

	float brushSize;

	editorState state;
	ShortcutManager shortcuts;
} Core;

#endif
