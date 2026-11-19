#include "../../include/systems/core.h"
#include "../../include/systems/renderer.h"
#include "../../include/systems/updater.h"
#include <stdlib.h>

Core* core = NULL;

bool plug_init(void) {
	core = calloc(1, sizeof(*core));
	if(core == NULL)
		return false;

	core->canvas.dimensions = (Vector2){GetScreenWidth(), GetScreenHeight()};
	create_layer(&core->canvas);
	if(core->canvas.layer_count == 0 || !IsRenderTextureValid(core->canvas.layers[0].tex)) {
		free(core->canvas.layers);
		free(core);
		core = NULL;
		return false;
	}
	return true;
}

void plug_render() {
	render(core);
}

void plug_update() {
	update(core);
}

void* plug_pre_reload(void) {
	return core;
}

void plug_post_reload(void* data) {
	core = data;
}
