#include "../../include/systems/renderer.h"

void render(Core* core) {
	Canvas* canvas = &core->canvas;
	editorState* state = &core->state;

	if(canvas == NULL)
		return;

	if(canvas->layers == NULL)
		return;

	if(state->selectedLayer >= canvas->layer_count)
		return;

	for(int i = canvas->layer_count - 1; i >= 0; --i)
		render_layer(state, canvas->layers[i]);

	int y = 55;
	for(int i = 0; i < canvas->layer_count; ++i){
		bool selected = i == state->selectedLayer;
		const char* label = TextFormat("%s%s", canvas->layers[i].layer_name, selected ? " [SELECTED]" : "");
		DrawText(label, 20, y, 20, selected ? GREEN : BLACK);
		y += 25;
	}

	DrawCircleLinesV(state->mousePos, state->brushSize, GRAY);
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
