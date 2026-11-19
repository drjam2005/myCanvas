#ifndef RENDERER_H
#define RENDERER_H

#include "../structs/canvas.h"
#include "../structs/layer.h"
#include "./editor_info.h"

void render(editorState* state, Canvas* canvas);
void render_layer(editorState* state, Layer layer);
#endif
