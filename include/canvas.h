#pragma once
#ifndef CANVAS_H
#define CANVAS_H

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
    float opacity;           
    std::vector<Color> pixels;
    Texture2D tex;

    Layer(int w, int h, bool whiteBackground = false)
        : width(w), height(h), opacity(1.0f), pixels(w*h)
    {
        Color fill = whiteBackground ? Color{255,255,255,255} : Color{0,0,0,0};
        std::fill(pixels.begin(), pixels.end(), fill);

        Image tempImg = {};
        tempImg.width = w;
        tempImg.height = h;
        tempImg.mipmaps = 1;
        tempImg.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        tempImg.data = pixels.data();

        tex = LoadTextureFromImage(tempImg);
    }

    ~Layer() { 
		UnloadTexture(tex);
	}
};

class Canvas {
private:
    std::vector<Layer> layers;
	std::deque<std::pair<size_t, std::vector<Color>>> undo;
	std::deque<std::pair<size_t, std::vector<Color>>> redo;
	std::unordered_map<char, Color> colors;

    MOUSE_STATE mouseState;
    Vector2 prevMousePos;
    int width;
    int height;
    float brushSize;
	float eraserSize;
	bool isBrush;

    Color clr;
	char transparency;
    size_t selectedLayer;
public:
    Canvas(int width, int height, size_t maxLayers);

    void Update();
    void Render();
    Layer& getCurrentLayer();
private:
    void createLayer(bool whiteBackground = false);
    void drawCircle(Vector2 pos);
    void drawLine(Vector2 v1, Vector2 v2);
};

#endif

