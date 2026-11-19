#ifndef UPDATER_H
#define UPDATER_H

#include "../structs/canvas.h"
#include "../structs/layer.h"
#include "./editor_info.h"

void update(editorState* state, Canvas* canvas);
void post_update(editorState* state);
void update_layer(editorState* state, Layer* layer);
#endif
