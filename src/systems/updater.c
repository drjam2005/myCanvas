#include "../../include/systems/updater.h"
#include "../../vendor/raylib/windows/include/raymath.h"

void update(editorState* state, Canvas* canvas) {

	state->mousePos = GetMousePosition();
	state->mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	state->mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	state->mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
	state->isPanning = state->mouseDown && IsKeyDown(KEY_SPACE);

	if(state->isPanning)
		state->origin = Vector2Subtract(state->origin, Vector2Subtract(state->mousePos, state->prevPos));


	if(state->selectedLayer >= canvas->layer_count)
		return;

	update_layer(state, &canvas->layers[state->selectedLayer]);

	if(IsKeyPressed(KEY_E)) {
		create_layer(canvas);
	}

	if(IsKeyPressed(KEY_W)) {
		state->selectedLayer = Clamp(state->selectedLayer + 1, 0, canvas->layer_count - 1);
	} 

	if(IsKeyPressed(KEY_S)) {
		state->selectedLayer = Clamp(state->selectedLayer - 1, 0, canvas->layer_count - 1);
	}

	post_update(state);
}

void post_update(editorState* state) {
	state->prevPos = state->mousePos;
} 

void update_layer(editorState* state, Layer* layer) {
	BeginTextureMode(layer->tex);

	if (state->mousePressed)
		state->prevPos = state->mousePos;
	if (state->mouseDown && !state->isPanning) {
		DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, BLACK);
		DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, BLACK);
		state->prevPos = state->mousePos;
	}

	EndTextureMode();
}
