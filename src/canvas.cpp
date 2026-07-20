#include <cstdio>
#include <iostream>
#include <cstring>

#include "SDLHandler.h"

#include "canvas.h"
#include "raylib.h"
#include "raygui.h"

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
	handle_dropped_files();

	if(droppedFile.length()) 
		return;

	if (!isPenInProximity && !IsTabletPenDown())
		pointerPos = GetMousePosition();

	handle_pen_events();
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

	bus.emptyEvents();
	
	if(this->droppedFile.length()) {
		ShowCursor();
		Rectangle rec = {GetScreenWidth()/2.0f - 150, 100, 300, 150};
		int result = GuiMessageBox(rec, 
				TextFormat("Unsaved Changes to %s", fileName.c_str()), 
				TextFormat("You have unsaved changes to %s\nDo you want to overwrite this?", fileName.c_str()), 
				"Yes;No"
			);

		if(result >= 0) {
			if(result == 1) {
				this->fileName = this->droppedFile;
				handle_file_loading();
			}
			this->droppedFile = "";

			HideCursor();
		} 
	}

}
