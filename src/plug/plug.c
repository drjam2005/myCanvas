#include "../../include/systems/core.h"
#include "../../include/systems/renderer.h"
#include "../../include/systems/updater.h"
#include "../../include/structs/canvas.h"
#include <stdlib.h>

#include "../../include/systems/inputs/inputs.h"

Core* core = NULL;

static bool register_shortcuts(ShortcutManager* shortcuts) {
	PEN_SHORTCUT(shortcuts, func_draw, MOUSE_LEFT_PRESSED | MOUSE_LEFT_DOWN);

	KEY_SHORTCUT(shortcuts, func_toggle_drawing_mode, KEY_E);
	KEY_SHORTCUT(shortcuts, func_create_layer, KEY_LEFT_CONTROL, KEY_E);
	KEY_SHORTCUT(shortcuts, func_select_layer_up, KEY_LEFT_CONTROL, KEY_W);
	KEY_SHORTCUT(shortcuts, func_select_layer_down, KEY_LEFT_CONTROL, KEY_S);

	return true;
}

bool plug_init(void) {
	if(core != NULL) {
		key_shortcut_free(&core->shortcuts);
		pen_shortcut_free(&core->shortcuts);
		free(core);
	}
	core = (Core*)calloc(1, sizeof(*core));
	core->canvas.dimensions = (Vector2){(int)GetScreenWidth(), (int)GetScreenHeight()};
	create_layer(&core->canvas, "background");
	create_layer(&core->canvas, "Layer 1"); core->state.selectedLayer = 1;

	// let's manually edit the layer "background" to be white
	Layer* layer = &core->canvas.layers[0];
	BeginTextureMode(layer->tex);
		ClearBackground(RAYWHITE);
	EndTextureMode();

	return register_shortcuts(&core->shortcuts);
}

void plug_render() {
	ClearBackground(GRAY);

	render(core);
}

void plug_update() {
	update(core);
}

void* plug_pre_reload(void) {
	return core;
}

bool plug_post_reload(void* data) {
	core = (Core*)data;
	// Re-register callbacks from the current copy of the shared library.
	key_shortcut_free(&core->shortcuts);
	pen_shortcut_free(&core->shortcuts);
	return register_shortcuts(&core->shortcuts);
}
