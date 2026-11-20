#include "../../../include/systems/inputs/inputs.h"
#include "../../../vendor/raylib/unix/include/rlgl.h"
#include <stdlib.h>
#include <string.h>

void func_draw(Core* core) {
	Canvas* canvas = &core->canvas;
	editorState* state = &core->state;

	BeginTextureMode(canvas->layers[state->selectedLayer].tex);

	if (state->pen_state & MOUSE_LEFT_PRESSED)
		state->prevPos = state->mousePos;
	if (state->pen_state & MOUSE_LEFT_DOWN && !state->isPanning) {
		if(state->isErasing) {
			rlSetBlendFactors(RL_ZERO, RL_ONE_MINUS_SRC_ALPHA, RL_SRC_ALPHA);
			rlSetBlendMode(BLEND_CUSTOM);
				DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, RED);
				DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, RED);
			rlSetBlendMode(BLEND_ALPHA);
		} else {
			DrawLineEx(Vector2Add(state->origin, state->prevPos), Vector2Add(state->origin, state->mousePos), 20.0f, state->currentColor);
			DrawCircleV(Vector2Add(state->origin, state->mousePos), 10.0f, state->currentColor);
		}
		state->prevPos = state->mousePos;
		core->stroke_changed = true;
	}

	EndTextureMode();

	if((state->pen_state & MOUSE_LEFT_RELEASED) && core->stroke_changed) {
		core->stroke_changed = false;
		save_snapshot(core);
	}
}

void func_toggle_drawing_mode(Core* core) {
	core->state.isErasing = !core->state.isErasing;
}

void func_create_layer(Core* core) {
	if(create_layer(&core->canvas, TextFormat("Layer %zu", core->canvas.layer_count)))
		save_snapshot(core);
}

void func_select_layer_up(Core* core) {
	if(IsKeyDown(KEY_LEFT_SHIFT) && core->state.selectedLayer > 0) {
		Layer temp = core->canvas.layers[core->state.selectedLayer];
		core->canvas.layers[core->state.selectedLayer] = core->canvas.layers[core->state.selectedLayer-1];
		core->canvas.layers[core->state.selectedLayer-1] = temp;
		core->state.selectedLayer -= 1;
		save_snapshot(core);
	} else
		core->state.selectedLayer = Clamp(core->state.selectedLayer - 1, 0, core->canvas.layer_count - 1);
}

void func_select_layer_down(Core* core) {
	if(IsKeyDown(KEY_LEFT_SHIFT) && core->state.selectedLayer + 1 < core->canvas.layer_count-1) {
		Layer temp = core->canvas.layers[core->state.selectedLayer];
		core->canvas.layers[core->state.selectedLayer] = core->canvas.layers[core->state.selectedLayer+1];
		core->canvas.layers[core->state.selectedLayer+1] = temp;
		core->state.selectedLayer += 1;
		save_snapshot(core);
	} else
		core->state.selectedLayer = Clamp(core->state.selectedLayer + 1, 0, core->canvas.layer_count - 1);
}

static void free_snapshot(CanvasSnapshot* snapshot) {
	for(size_t i = 0; i < snapshot->layer_count; ++i)
		UnloadImage(snapshot->layers[i].img);
	free(snapshot->layers);
	*snapshot = (CanvasSnapshot){0};
}

void clear_snapshots(Core* core) {
	for(size_t i = 0; i < core->snapshot_count; ++i)
		free_snapshot(&core->snapshots[i]);
	core->snapshot_count = 0;
	core->snapshot_index = 0;
}

bool save_snapshot(Core* core) {
	CanvasSnapshot snapshot = {0};
	snapshot.layers = calloc(core->canvas.layer_count, sizeof(*snapshot.layers));
	if(snapshot.layers == NULL)
		return false;

	snapshot.selected_layer = core->state.selectedLayer;
	for(size_t i = 0; i < core->canvas.layer_count; ++i) {
		const Layer* layer = &core->canvas.layers[i];
		LayerSnapshot* saved = &snapshot.layers[i];

		saved->img = LoadImageFromTexture(layer->tex.texture);
		if(!IsImageValid(saved->img)) {
			free_snapshot(&snapshot);
			return false;
		}
		++snapshot.layer_count;
		memcpy(saved->layer_name, layer->layer_name, sizeof(saved->layer_name));
		saved->blending_mode = layer->blending_mode;
		saved->opacity = layer->opacity;
	}

	size_t first = core->snapshot_index;
	size_t count = core->snapshot_count - first;
	for(size_t i = 0; i < first; ++i)
		free_snapshot(&core->snapshots[i]);
	if(count == MAX_UNDO_HISTORY) {
		free_snapshot(&core->snapshots[--count]);
	}

	memmove(core->snapshots + 1, core->snapshots + first,
	        count * sizeof(*core->snapshots));
	core->snapshots[0] = snapshot;
	core->snapshot_count = count + 1;
	core->snapshot_index = 0;
	return true;
}

static void restore_snapshot(Core* core, size_t index) {
	const CanvasSnapshot* snapshot = &core->snapshots[index];
	Layer* layers = calloc(snapshot->layer_count, sizeof(*layers));
	if(layers == NULL)
		return;

	for(size_t i = 0; i < snapshot->layer_count; ++i) {
		const LayerSnapshot* saved = &snapshot->layers[i];
		layers[i] = createLayer((Vector2){saved->img.width, saved->img.height}, saved->layer_name);
		if(!IsRenderTextureValid(layers[i].tex) || layers[i].tex.texture.format != saved->img.format) {
			for(size_t j = 0; j <= i; ++j)
				UnloadRenderTexture(layers[j].tex);
			free(layers);
			return;
		}
		UpdateTexture(layers[i].tex.texture, saved->img.data);
		layers[i].blending_mode = saved->blending_mode;
		layers[i].opacity = saved->opacity;
	}

	for(size_t i = 0; i < core->canvas.layer_count; ++i)
		UnloadRenderTexture(core->canvas.layers[i].tex);
	free(core->canvas.layers);
	core->canvas.layers = layers;
	core->canvas.layer_count = snapshot->layer_count;
	core->state.selectedLayer = snapshot->selected_layer;
	core->snapshot_index = index;
}

void func_undo(Core* core) {
	if(!core->stroke_changed && core->snapshot_index + 1 < core->snapshot_count)
		restore_snapshot(core, core->snapshot_index + 1);
}

void func_redo(Core* core) {
	if(!core->stroke_changed && core->snapshot_index > 0)
		restore_snapshot(core, core->snapshot_index - 1);
}
