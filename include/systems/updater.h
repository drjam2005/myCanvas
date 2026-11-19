#ifndef UPDATER_H
#define UPDATER_H

#include "../systems/core.h"
#include "./editor_info.h"

void update(Core* core);
void post_update(editorState* state);
void update_layer(editorState* state, Layer* layer);

#endif
