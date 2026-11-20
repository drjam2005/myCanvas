#include "../../include/systems/core.h"
#include "../../include/systems/renderer.h"
#include "../../include/systems/updater.h"
#include "../../include/structs/canvas.h"
#include <stdlib.h>

#include "../../include/systems/inputs/inputs.h"

Core* core = NULL;

static bool register_shortcuts(ShortcutManager* shortcuts) {
	if (PEN_SHORTCUT(shortcuts, draw, MOUSE_LEFT_PRESSED | MOUSE_LEFT_DOWN) &&
	    KEY_SHORTCUT(shortcuts, toggleDrawing, KEY_E))
		return true;

	key_shortcut_free(shortcuts);
	pen_shortcut_free(shortcuts);
	return false;
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
