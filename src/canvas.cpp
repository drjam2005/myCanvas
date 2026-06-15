#include <cstdio>
#include <cstring>
#include <cmath>

#include "canvas.h"
#include "raylib.h"

Canvas::Canvas(int width, int height, size_t maxLayers, std::string fileName)
    : width(width), height(height),
      brushSize(20.0f), eraserSize(20.0f), selectedLayer(0),
      mouseState(IDLE), prevMousePos({-1,-1}), transparency(255),
      fileName(fileName), clr(BLACK), isBrush(true), scale(1.0f),
	  isMirror(false), isColorPicking(false)
{
	handle_file_loading(); // file loading
	handle_window(); // set window stuff hmhmhmm
}

void Canvas::Update() {
	pointerPos = GetMousePosition();

	if (handle_pen_events()) return;
	if (handle_key_events()) return;
	
	handle_tool_input();
	prevMousePos = GetMousePos();
}

void Canvas::Render() {
	render_layers();
	if(isUiHidden)
		return;
	render_color_picker();
	render_layer_ui();
}
