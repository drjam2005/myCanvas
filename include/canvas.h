#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <deque>
#include <unordered_map>
#include "raylib.h"

enum MOUSE_STATE {
    HELD,
    IDLE
};

struct Layer {
    int width;
    int height;
    unsigned char opacity = 255;
    RenderTexture2D tex;
	BlendMode blendingMode;

    Layer(int w, int h, bool whiteBackground = false)
        : width(w), height(h), opacity(255), blendingMode(BLEND_ALPHA)
    {
        tex = LoadRenderTexture(w, h);
		BeginTextureMode(tex);
		//DrawRectangle(0, 0, tex.texture.width, tex.texture.height, Color{0,0,0,0});
		ClearBackground(BLANK);
		opacity = 255;

		if(whiteBackground){
			BeginTextureMode(tex);
			DrawRectangle(0, 0, tex.texture.width, tex.texture.height, WHITE);
		}
		EndTextureMode();
    }

    ~Layer() { 
		UnloadRenderTexture(tex);
	}
};

class Canvas {
private:
	std::string fileName;
    std::deque<Layer> layers;
	std::deque<std::pair<size_t, Image>> undo;
	std::deque<std::pair<size_t, Image>> redo;
	std::deque<Color> colorQueue;

    MOUSE_STATE mouseState;
    Vector2 prevMousePos;
	Vector2 canvasPos = {0, 0};
	Rectangle colorPickerRec;
	Rectangle colorPickerBounds;
    int width;
    int height;
    float brushSize;
	float eraserSize;
	float scale;
	bool isBrush;
	bool isMirror;
	bool isColorPicking;

    Color clr;
	char transparency;
    size_t selectedLayer;
public:
    Canvas(int width, int height, size_t maxLayers, std::string fileName);
	Canvas() {}

    void Update();
    void Render();
    Layer& getCurrentLayer();
private:
    void createLayer(bool whiteBackground = false);
    void drawCircle(Vector2 pos);
    void drawLine(Vector2 v1, Vector2 v2);
};

#endif
