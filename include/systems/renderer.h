#ifndef RENDERER_H
#define RENDERER_H

#include "../systems/core.h"
#include "./editor_info.h"

void render(Core* core);
void render_layer(editorState* state, Layer layer);
#endif
