#ifndef INPUTS_H
#define INPUTS_H

#include "../../systems/core.h"
#include "../editor_info.h"

void func_draw(Core* core);
void func_toggle_drawing_mode(Core* core);
void func_create_layer(Core* core);

void func_select_layer_up(Core* core);
void func_select_layer_down(Core* core);

bool save_snapshot(Core* core);
void clear_snapshots(Core* core);

void func_undo(Core* core);
void func_redo(Core* core);

#endif
