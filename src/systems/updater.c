#include "../../include/systems/updater.h"
#include "../../vendor/raylib/windows/include/raymath.h"

void update(Core* core) {
	Canvas* canvas = &core->canvas;
	editorState* state = &core->state;

	state->mousePos = GetMousePosition();
	state->mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
	state->mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
	state->mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
	state->isPanning = state->mouseDown && IsKeyDown(KEY_SPACE);
	state->pen_state = PEN_NULL;

	state->brushSize += GetMouseWheelMove();

	if (state->mousePressed)
		state->pen_state |= MOUSE_LEFT_PRESSED;
	else if (state->mouseReleased)
		state->pen_state |= MOUSE_LEFT_RELEASED;
	else if (state->mouseDown)
		state->pen_state |= MOUSE_LEFT_DOWN;

	if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
		state->pen_state |= MOUSE_RIGHT_PRESSED;
	else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
		state->pen_state |= MOUSE_RIGHT_RELEASED;
	else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
		state->pen_state |= MOUSE_RIGHT_DOWN;

	if(state->isPanning)
		state->origin = Vector2Subtract(state->origin, Vector2Subtract(state->mousePos, state->prevPos));

	if(state->selectedLayer >= canvas->layer_count)
		return;

	key_shortcut_update(&core->shortcuts, core);
	pen_shortcut_update(&core->shortcuts, core);

	post_update(state);
}

void post_update(editorState* state) {
	state->prevPos = state->mousePos;
}

void update_layer(editorState* state, Layer* layer) {
}
