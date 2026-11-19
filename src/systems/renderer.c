#include "../../include/systems/renderer.h"

void render(editorState* state, Canvas* canvas) {
	if(canvas == NULL)
		return;

	if(canvas->layers == NULL)
		return;

	if(state->selectedLayer >= canvas->layer_count)
		return;

	for(int i = 0; i < canvas->layer_count; ++i) {
		render_layer(state, canvas->layers[i]);
	}

	DrawText(TextFormat("S: (%zu), C: (%zu)", state->selectedLayer, canvas->layer_count), 20, 20, 30, BLACK);
}

void render_layer(editorState* state, Layer layer) {
	DrawTexturePro(
		layer.tex.texture,
		(Rectangle){0, 0, layer.tex.texture.width, -layer.tex.texture.height},
		(Rectangle){0, 0, layer.tex.texture.width, layer.tex.texture.height},
		state->origin,
		state->rotation,
		WHITE
	);
}
