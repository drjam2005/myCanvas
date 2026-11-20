#include "../../include/systems/core.h"
#include "../../include/systems/renderer.h"
#include "../../include/systems/updater.h"
#include "../../include/structs/canvas.h"
#include <stdlib.h>

#include "../../include/systems/inputs/inputs.h"

Core* core = NULL;

static bool register_draw_shortcuts(ShortcutManager* shortcuts) {
	return PEN_SHORTCUT(shortcuts, draw, PEN_PRESSED) &&
	       PEN_SHORTCUT(shortcuts, draw, PEN_DOWN);
}

bool plug_init(void) {
	if(core != NULL) {
		free(core->shortcuts.pen_shortcuts);
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

	return register_draw_shortcuts(&core->shortcuts);
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

void plug_post_reload(void* data) {
	core = (Core*)data;
	// Re-register callbacks from the current copy of the shared library.
	free(core->shortcuts.pen_shortcuts);
	core->shortcuts.pen_shortcuts = NULL;
	core->shortcuts.penshortcut_count = 0;
	register_draw_shortcuts(&core->shortcuts);
}
