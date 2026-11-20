#include "../../include/structs/layer.h"

Layer createLayer(Vector2 dims, const char* layer_name) {
	Layer newLayer = {0};

	newLayer.tex = LoadRenderTexture(dims.x, dims.y);
	if(IsRenderTextureValid(newLayer.tex)) {
		BeginTextureMode(newLayer.tex);
		ClearBackground(BLANK);
		EndTextureMode();
	}
	newLayer.layer_name = layer_name;
	newLayer.blending_mode = BLEND_ALPHA;
	newLayer.opacity = (char)255;

	return newLayer;
}
