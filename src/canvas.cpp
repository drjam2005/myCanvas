#include "canvas.h"
#include <iostream>
#include "raylib.h"
#include <algorithm>
#include <cmath>

Canvas::Canvas(int width, int height, size_t maxLayers)
    : width(width), height(height),
      brushSize(20.0f), eraserSize(20.0f), selectedLayer(0),
      mouseState(IDLE), prevMousePos({-1,-1}), transparency(255),
      clr(BLACK)
{
	// setting colors
		this->colors['1'] = BLACK;
		this->colors['2'] = RED;
		this->colors['3'] = BLUE;
		this->colors['4'] = GREEN;
		this->colors['5'] = ORANGE;
		// lazy to add more

	isBrush = true;
	layers.reserve(maxLayers);
    createLayer(true);
}

void Canvas::createLayer(bool whiteBackground) {
    layers.emplace_back(width, height, whiteBackground);
}

Layer& Canvas::getCurrentLayer() {
    return layers[selectedLayer];
}

void Canvas::drawCircle(Vector2 v1) {
    Layer& layer = getCurrentLayer();
	float r = isBrush ? brushSize : eraserSize;
    float r2 = r*r;

    int minX = std::max(0, (int)(v1.x - r));
    int maxX = std::min(layer.width - 1, (int)(v1.x + r));
    int minY = std::max(0, (int)(v1.y - r));
    int maxY = std::min(layer.height - 1, (int)(v1.y + r));

    for(int y = minY; y <= maxY; ++y) {
        for(int x = minX; x <= maxX; ++x) {
            float dx = x - v1.x;
            float dy = y - v1.y;
            if(dx*dx + dy*dy <= r2) {
                layer.pixels[y*layer.width + x] = clr;
            }
        }
    }
}

void Canvas::drawLine(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    float len = sqrtf(dx*dx + dy*dy);
    if(len == 0) return;

    float stepX = dx / len;
    float stepY = dy / len;

    Vector2 curr = v1;
    for(int i = 0; i <= (int)len; ++i) {
        drawCircle(curr);
        curr.x += stepX;
        curr.y += stepY;
    }
}

void Canvas::Update() {
    Vector2 mousePos = GetMousePosition();
	if(IsKeyDown(KEY_LEFT_SHIFT)){
		if(IsKeyPressed(KEY_Z)){
			if (!redo.empty()) {
				size_t layerIdx = redo.top().first;
				
				undo.push({layerIdx, layers[layerIdx].pixels});
				
				layers[layerIdx].pixels = redo.top().second;
				redo.pop();

				UpdateTexture(layers[layerIdx].tex, layers[layerIdx].pixels.data());
			}
		}
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			prevMousePos = GetMousePosition();

		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			Vector2 currPos = GetMousePosition();
			float diff = 0.25f*(currPos.x - prevMousePos.x);
			prevMousePos = currPos;
			if(isBrush)
				brushSize = fmax(1.0f, fmin(brushSize + diff, 50.0f));
			else
				eraserSize = fmax(1.0f, fmin(eraserSize + diff, 50.0f));
		}
		return;
	}

	if(IsKeyDown(KEY_LEFT_CONTROL)){
		if(IsKeyPressed(KEY_E)){
			createLayer(false);
			return;
		}
		if(IsKeyPressed(KEY_W)){
			selectedLayer = (selectedLayer + 1) % layers.size();
		}
		if (IsKeyPressed(KEY_S)) {
			if (selectedLayer == 0) {
				selectedLayer = layers.size() - 1;
			} else {
				selectedLayer--;
			}
		}
		if (IsKeyPressed(KEY_Z)) {
				if (!undo.empty()) {
					size_t layerIdx = undo.top().first;
					
					redo.push({layerIdx, layers[layerIdx].pixels});

					layers[layerIdx].pixels = undo.top().second;
					undo.pop();

					UpdateTexture(layers[layerIdx].tex, layers[layerIdx].pixels.data());
					mouseState = IDLE;
			}
			return;
		}
		if(IsKeyPressed(KEY_ONE))
			transparency = 255/4;
		if(IsKeyPressed(KEY_TWO))
			transparency = (255/4) * 2;
		if(IsKeyPressed(KEY_THREE))
			transparency = (255/4) * 3;
		if(IsKeyPressed(KEY_FOUR))
			transparency = 255;
		if (isBrush) 
			clr.a = transparency;

		return;
	}

	if(IsKeyPressed(KEY_E)){
		if(clr.a != 0){
			clr.a = 0;
			isBrush = false;
		}else{
			clr.a = transparency;
			isBrush = true;
		}
	}

	// colors
	for(auto c : colors){
		if(IsKeyPressed(c.first)){
			clr = c.second;
			clr.a = transparency;
		}
	}

	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		undo.push({selectedLayer, getCurrentLayer().pixels});
		while(!redo.empty())
			redo.pop();
	}
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        if(mouseState == HELD && prevMousePos.x >= 0) {
            drawLine(prevMousePos, mousePos);
        } else {
            drawCircle(mousePos);
        }
        prevMousePos = mousePos;
        mouseState = HELD;
    } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        mouseState = IDLE;
        prevMousePos = {-1,-1};
    }

    for(auto& l : layers) {
        UpdateTexture(l.tex, l.pixels.data());
    }

}

void Canvas::Render() {
	BeginBlendMode(BLEND_ALPHA);
	for(auto& l : layers) {
		DrawTexture(l.tex, 0, 0, WHITE);
    }
	EndBlendMode();
	DrawCircleLinesV(GetMousePosition(), isBrush ? brushSize : eraserSize, GRAY);
	// draw colors
	DrawText("Color Bindings: ", 20, 20, 20, BLACK);
	size_t x = colors.size();
	for(auto c : colors){
		DrawText(TextFormat("%d", c.first-'0'), 20+(20*x), 40, 20, c.second);
		if(c.second.r == clr.r &&
		   c.second.g == clr.g &&
		   c.second.b == clr.b)
			DrawText("_", 20+(20*x), 43, 20, BLACK);
		x--;
	}
	
	for(int i = layers.size()-1, y = 0; i >= 0; i--, y++){
		DrawText(TextFormat("Layer: %d", i), 20, 80+(20*y), 20, i == selectedLayer ? GREEN : BLACK);
	}
	DrawText((isBrush ? "Current Mode: BRUSH" : "Current Mode: ERASER"), 20, GetScreenHeight()-60.0f, 20, BLACK);
	DrawText(TextFormat("Transparency: %.0f", ((float)clr.a/255.0f)*100.0f), 20, GetScreenHeight()-40.0f, 20, BLACK);
}

