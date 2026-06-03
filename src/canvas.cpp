#include <cstdio>
#include <cstring>
#include <cmath>

#include "canvas.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

Canvas::Canvas(int width, int height, size_t maxLayers, std::string fileName)
    : width(width), height(height),
      brushSize(20.0f), eraserSize(20.0f), selectedLayer(0),
      mouseState(IDLE), prevMousePos({-1,-1}), transparency(255),
      fileName(fileName), clr(BLACK), isBrush(true), scale(1.0f),
	  isMirror(false), isColorPicking(false)
{
	pressure = 1.0f;
	canvasDimensions.x = 0;
	canvasDimensions.y = 0;
	canvasDimensions.width = width;
	canvasDimensions.height = height;

	if (load(fileName)) {
		selectedLayer = layers.size() - 1;
		SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
	} else {
		layers.clear(); 

		create_layer(true);  
		create_layer(false); 

		selectedLayer = 1;
		SetWindowTitle("myCanvas | [NEW]");
	}

	// set window stuff hmhmhmm
	int monitor = GetCurrentMonitor();
	int monitorWidth  = GetMonitorWidth(monitor);
	int monitorHeight = GetMonitorHeight(monitor);

	int windowWidth  = monitorWidth * 0.8f;
	int windowHeight = monitorHeight * 0.8f;

	SetWindowSize(windowWidth, windowHeight);
	SetWindowPosition(
		(monitorWidth  - windowWidth)  / 2,
		(monitorHeight - windowHeight) / 2
	);

	if(width > height)
		scale = 0.9f*((float)GetScreenHeight()/height);
	else if(height > width)
		scale = 0.9f*((float)GetScreenWidth()/width);
	else{
		if(width > 0.9f*GetScreenWidth())
			scale = 0.9f*((float)GetScreenWidth()/width);
		else if(height > 0.9f*GetScreenHeight())
			scale = 0.9f*((float)GetScreenHeight()/height);
		else
			scale = 0.9f*((float)GetScreenHeight()/height);
	}

	canvasPos = {
		(GetScreenWidth()  - (scale*width))  * 0.5f,
		(GetScreenHeight() - (scale*height)) * 0.5f
	};

	if(colorQueue.size() <= 0)
		colorQueue.push_front(BLACK);
	else{
		clr = colorQueue[0];
	}

}

void Canvas::create_layer(bool whiteBackground) {
    layers.emplace_back(width, height, whiteBackground);
}

Layer& Canvas::get_current_layer() {
    return layers[selectedLayer];
}

Color Canvas::pick_color(Vector2 v1){
	Color clr;
	int xpos = (int)(screen_to_canvas(v1).x);
	int ypos = height - (int)(screen_to_canvas(v1).y) - 1;
	clr = GetImageColor(currentLayerCache, xpos, ypos);

	return clr;
}

void Canvas::draw_circle(Vector2 v1) {
    float r = (isBrush ? brushSize : eraserSize) * pressure;
    BeginTextureMode(layers[selectedLayer].tex);
    
    if (!isBrush) {
        rlSetBlendFactors(RL_ZERO, RL_ONE_MINUS_SRC_ALPHA, RL_SRC_ALPHA);
        rlSetBlendMode(BLEND_CUSTOM);
        DrawCircleV(Vector2{v1.x, GetScreenHeight() - v1.y}, r, WHITE);
        rlSetBlendMode(BLEND_ALPHA); 
    } else {
        DrawCircleV(Vector2{v1.x, GetScreenHeight() - v1.y}, r, clr);
    }
    EndTextureMode();
}

Vector2 Canvas::screen_to_canvas(Vector2 pos) {
    Vector2 screenCenter = { (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };

    Vector2 result = Vector2Subtract(pos, screenCenter);

    result = Vector2Rotate(result, -rotation);

    result.x -= (canvasPos.x - screenCenter.x);
    result.y -= (canvasPos.y - screenCenter.y);

    if (isMirror) {
        float canvasWidthScaled = width * scale;
        result.x = canvasWidthScaled - result.x;
    }

    result.x /= scale;
    result.y /= scale;

    return result;
}

void Canvas::draw_line(Vector2 canvasFrom, Vector2 canvasTo) {
    BeginTextureMode(layers[selectedLayer].tex);

    if (!isBrush) {
        rlSetBlendFactors(RL_ZERO, RL_ONE_MINUS_SRC_ALPHA, RL_SRC_ALPHA);
        rlSetBlendMode(BLEND_CUSTOM);
    }

    float r = (isBrush ? brushSize : eraserSize) * pressure;
	DrawCircleV(canvasFrom, r, isBrush ? clr : WHITE);
	DrawLineEx(canvasFrom, canvasTo, 2*r, isBrush ? clr : WHITE);
	DrawCircleV(canvasTo, r, isBrush ? clr : WHITE);

	if(!isBrush)
		rlSetBlendMode(BLEND_ALPHA);
    EndTextureMode();
}

Vector2 Canvas::GetMousePos(){
	return pointerPos;
}

void Canvas::Update() {
	pointerPos = GetMousePosition();

	if (handle_pen_events()) return;
	if (handle_key_events()) return;
	
	handle_tool_input();
	prevMousePos = GetMousePos();
}
