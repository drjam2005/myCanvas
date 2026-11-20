#include "../../include/systems/core.h"
#include "../../include/systems/renderer.h"
#include "../../include/systems/updater.h"
#include "../../include/structs/canvas.h"
#include <stdlib.h>

#include "../../include/systems/inputs/inputs.h"

Core* core = NULL;

static bool register_shortcuts(ShortcutManager* shortcuts) {
	PEN_SHORTCUT(shortcuts, func_draw, MOUSE_LEFT_PRESSED | MOUSE_LEFT_DOWN | MOUSE_LEFT_RELEASED);

	KEY_SHORTCUT(shortcuts, func_toggle_drawing_mode, KEY_E);
	KEY_SHORTCUT(shortcuts, func_create_layer, KEY_LEFT_CONTROL, KEY_E);
	KEY_SHORTCUT(shortcuts, func_select_layer_up, KEY_LEFT_CONTROL, KEY_W);
	KEY_SHORTCUT(shortcuts, func_select_layer_down, KEY_LEFT_CONTROL, KEY_S);

	KEY_SHORTCUT(shortcuts, func_undo, KEY_LEFT_CONTROL, KEY_Z);
	KEY_SHORTCUT(shortcuts, func_redo, KEY_LEFT_CONTROL, KEY_LEFT_SHIFT, KEY_Z);

	return true;
}

bool plug_init(void) {
	if(core != NULL) {
		key_shortcut_free(&core->shortcuts);
		pen_shortcut_free(&core->shortcuts);
		clear_snapshots(core);
		for(size_t i = 0; i < core->canvas.layer_count; ++i)
			UnloadRenderTexture(core->canvas.layers[i].tex);
		free(core->canvas.layers);
		free(core);
	}
	core = (Core*)calloc(1, sizeof(*core));
	if(core == NULL)
		return false;
	core->canvas.dimensions = (Vector2){(int)GetScreenWidth(), (int)GetScreenHeight()};
	core->state.currentColor = BLACK;
	if(!create_layer(&core->canvas, "Background") || !create_layer(&core->canvas, "Layer 1"))
		return false;

	// let's manually edit the layer "background" to be white
	Layer* layer = &core->canvas.layers[1];
	BeginTextureMode(layer->tex);
		ClearBackground(RAYWHITE);
	EndTextureMode();

	return save_snapshot(core) && register_shortcuts(&core->shortcuts);
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
