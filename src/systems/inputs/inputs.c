#include "../../../include/systems/inputs/inputs.h"

void draw(Core* core) {
	Canvas* canvas = &core->canvas;
	editorState* state = &core->state;

	BeginTextureMode(canvas->layers[state->selectedLayer].tex);

	if (state->mousePressed)
		state->prevPos = state->mousePos;
	if (state->mouseDown && !state->isPanning) {
		DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, BLACK);
		DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, BLACK);
		state->prevPos = state->mousePos;
	}

	EndTextureMode();
}
