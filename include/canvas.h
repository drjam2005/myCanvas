#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <raylib.h>
#include <deque>
#include <unordered_map>
#include <vector>

enum MOUSE_STATE {
    HELD,
    IDLE
};

struct Layer {
    int width;
    int height;
    char opacity;           
    RenderTexture2D tex;
	BlendMode blendingMode;

    Layer(int w, int h, bool whiteBackground = false)
        : width(w), height(h), opacity(1.0f), blendingMode(BLEND_ALPHA)
    {
        tex = LoadRenderTexture(w, h);
		BeginTextureMode(tex);
		//DrawRectangle(0, 0, tex.texture.width, tex.texture.height, Color{0,0,0,0});
		ClearBackground(BLANK);

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
	std::unordered_map<char, Color> colors;

    MOUSE_STATE mouseState;
    Vector2 prevMousePos;
	Vector2 canvasPos = {0, 0};
	Rectangle canvasDimensions;
    int width;
    int height;
    float brushSize;
	float eraserSize;
	float scale;
	bool isBrush;

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
