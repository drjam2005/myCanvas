#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include <string>
#include <deque>
#include "raylib.h"
#include <SDLHandler.h>

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
		ClearBackground(BLANK);
		opacity = 255;

		if(whiteBackground){
			BeginTextureMode(tex);
			DrawRectangle(0, 0, tex.texture.width, tex.texture.height, WHITE);
		}
		EndTextureMode();
    }

	Layer(const Layer&) = delete;
	Layer& operator=(const Layer&) = delete;

	Layer(Layer&& other) noexcept
		: width(other.width),
		  height(other.height),
		  opacity(other.opacity),
		  tex(other.tex),
		  blendingMode(other.blendingMode)
	{
		other.tex = {};
	}

	Layer& operator=(Layer&& other) noexcept
	{
		if (this != &other) {
			if (tex.id != 0) {
				UnloadRenderTexture(tex);
			}

			width = other.width;
			height = other.height;
			opacity = other.opacity;
			tex = other.tex;
			blendingMode = other.blendingMode;
			other.tex = {};
		}
		return *this;
	}

    ~Layer() { 
		if (tex.id != 0) {
			UnloadRenderTexture(tex);
		}
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
    Vector2 prevMousePos = {-1, -1};
	Vector2 pointerPos = {0, 0};
	Vector2 pressedMousePos = {-1, -1};
	Vector2 canvasPos = {0, 0};
	Rectangle colorPickerRec;
	Rectangle colorPickerBounds;
	Rectangle canvasDimensions;
	float rotation = 0.0f;
	Image currentLayerCache;
    int width;
    int height;
    float brushSize;
	float pressure;
	float eraserSize;
	float scale;
	bool isBrush;
	bool isPenInProximity = false;
	bool isPenDown = false;
	bool isMirror;
	bool isColorPicking = true;

    Color clr;
    Color previewClr;
	unsigned char transparency;
    size_t selectedLayer;
public:
    Canvas(int width, int height, size_t maxLayers, std::string fileName);
	Canvas() {}
	Canvas(const Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;

    void Update();
    void Render();
private:
	Color pick_color(Vector2 pos);
	Vector2 screen_to_canvas(Vector2 pos);
	Vector2 GetMousePos();
    Layer& get_current_layer();
    void create_layer(bool whiteBackground = false);
    void draw_circle(Vector2 pos);
    void draw_line(Vector2 v1, Vector2 v2);

	// startup
	void handle_file_loading();
	void handle_window();
	bool load(std::string fileName);

	// misc
	void save_to_png();
	void save();

	// Update Stuff
	bool penPressedThisFrame = false;
	bool penReleasedThisFrame = false;

	bool pointerPressed = false;
	bool pointerDown = false;
	bool pointerReleased = false;

	bool handle_pen_events();
	bool handle_key_events();
	bool handle_tool_input();

	// Rendering Stuff
	void render_layers();
	void render_color_picker();
	void render_layer_ui();
};

#endif // CANVAS_H
