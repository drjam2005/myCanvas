#include "../../include/structs/layer.h"

Layer createLayer(Vector2 dims) {
	Layer newLayer = {0};

	newLayer.tex = LoadRenderTexture(dims.x, dims.y);
	if(IsRenderTextureValid(newLayer.tex)) {
		BeginTextureMode(newLayer.tex);
		ClearBackground(BLANK);
		EndTextureMode();
	}
	newLayer.blending_mode = BLEND_ALPHA;
	newLayer.opacity = (char)255;

	return newLayer;
}
