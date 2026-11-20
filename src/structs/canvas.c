#include "../../include/structs/canvas.h"
#include <string.h>
#include <stdlib.h>

Layer* create_layer(Canvas* canvas, const char* layer_name) {
	if(canvas == NULL)
		return NULL;

	Layer* temp = realloc(canvas->layers, sizeof(Layer)*(canvas->layer_count+1));
	if(temp == NULL) 
		return NULL;
	canvas->layers = temp;
	memmove(canvas->layers + 1, canvas->layers, (canvas->layer_count)*(sizeof(Layer)));
	canvas->layer_count++;
	canvas->layers[0] = createLayer(canvas->dimensions, layer_name);

	return &canvas->layers[0];
}
