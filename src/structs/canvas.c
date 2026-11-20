#include "../../include/structs/canvas.h"
#include <stdlib.h>

void create_layer(Canvas* canvas, const char* layer_name) {
	if(canvas == NULL)
		return;

	void* temp = realloc(canvas->layers, sizeof(Layer)*(canvas->layer_count+1));
	if(temp == NULL)
		return;
	canvas->layer_count++;
	canvas->layers = temp;
	canvas->layers[canvas->layer_count-1] = createLayer(canvas->dimensions, layer_name);
}
