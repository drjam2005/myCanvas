#include "../../../include/systems/inputs/inputs.h"

void func_draw(Core* core) {
	Canvas* canvas = &core->canvas;
	editorState* state = &core->state;

	BeginTextureMode(canvas->layers[state->selectedLayer].tex);

	if (state->mousePressed)
		state->prevPos = state->mousePos;
	if (state->mouseDown && !state->isPanning) {
		if(state->isErasing) {
			DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, RED);
			DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, RED);
		} else {
			DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, BLACK);
			DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, BLACK);
		}
		state->prevPos = state->mousePos;
	}

	EndTextureMode();
}

void func_toggle_drawing_mode(Core* core) {
	core->state.isErasing = !core->state.isErasing;
}

void func_create_layer(Core* core) {
	create_layer(&core->canvas, TextFormat("Layer %zu", core->canvas.layer_count));
}

void func_select_layer_up(Core* core) {
	if(IsKeyDown(KEY_LEFT_SHIFT) && core->state.selectedLayer > 0) {
		Layer temp = core->canvas.layers[core->state.selectedLayer];
		core->canvas.layers[core->state.selectedLayer] = core->canvas.layers[core->state.selectedLayer-1];
		core->canvas.layers[core->state.selectedLayer-1] = temp;
		core->state.selectedLayer -= 1;
	} else
		core->state.selectedLayer = Clamp(core->state.selectedLayer - 1, 0, core->canvas.layer_count - 1);
}

void func_select_layer_down(Core* core) {
	if(IsKeyDown(KEY_LEFT_SHIFT) && core->state.selectedLayer + 1 < core->canvas.layer_count-1) {
		Layer temp = core->canvas.layers[core->state.selectedLayer];
		core->canvas.layers[core->state.selectedLayer] = core->canvas.layers[core->state.selectedLayer+1];
		core->canvas.layers[core->state.selectedLayer+1] = temp;
		core->state.selectedLayer += 1;
	} else
		core->state.selectedLayer = Clamp(core->state.selectedLayer + 1, 0, core->canvas.layer_count - 1);
}
