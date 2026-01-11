#include <iostream>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "canvas.h"
#include "raylib.h"

Canvas::Canvas(int width, int height, size_t maxLayers, std::string fileName)
    : width(width), height(height),
      brushSize(20.0f), eraserSize(20.0f), selectedLayer(0),
      mouseState(IDLE), prevMousePos({-1,-1}), transparency(255),
      fileName(fileName), clr(BLACK)
{
    layers.reserve(maxLayers);
    bool loadedSuccessfully = false;

	if(fileName != ""){
		std::ifstream file(fileName, std::ios::binary);
		if(file.is_open()) {
			int w, h, layerCount;
			std::string header;
			std::getline(file, header);
			sscanf(header.c_str(), "%d %d %d", &w, &h, &layerCount);

			width  = w;
			height = h;
			SetWindowSize(w, h);

			layers.clear();

			for (int i = 0; i < layerCount; i++) {
				std::string meta;
				std::getline(file, meta);

				int opacityInt, blendMode, compressedSize;
				sscanf(meta.c_str(), "%d %d %d",
						&opacityInt,
						&blendMode,
						&compressedSize);

				std::vector<unsigned char> compressedBuffer(compressedSize);
				file.read((char*)compressedBuffer.data(), compressedSize);

				file.ignore(1, '\n');

				int decompressedSize = 0;
				unsigned char* decompressed =
					DecompressData(compressedBuffer.data(),
							compressedSize,
							&decompressedSize);

				createLayer(false);
				Layer& l = layers.back();

				l.opacity = (unsigned char)opacityInt;
				l.blendingMode = (BlendMode)blendMode;

				size_t maxBytes = l.pixels.size() * sizeof(Color);
				memcpy(l.pixels.data(),
						decompressed,
						std::min((size_t)decompressedSize, maxBytes));

				UpdateTexture(l.tex, l.pixels.data());
				MemFree(decompressed);
			}
			loadedSuccessfully = true;
		}

	}

    if(!loadedSuccessfully) {
        createLayer(true);
        SetWindowTitle("myCanvas | [NEW]");
    } else {
        SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
    }

	colors['1'] = BLACK;
	colors['2'] = RED;
	colors['3'] = GREEN;
	colors['4'] = BLUE;
	colors['5'] = ORANGE;
    isBrush = true;
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
	bool ctrl  = IsKeyDown(KEY_LEFT_CONTROL);
	bool shift = IsKeyDown(KEY_LEFT_SHIFT);
	if (IsKeyPressed(KEY_ENTER)) {
		if (fileName == "") return;

		std::ofstream file(fileName, std::ios::binary);
		if (!file.is_open()) return;

		file << width << " " << height << " " << layers.size() << "\n";
		for (auto& l : layers) {
			int compressedSize = 0;

			unsigned char opacity = (unsigned char)l.opacity;
			int blendMode = (int)l.blendingMode;

			unsigned char* compressedData =
				CompressData((unsigned char*)l.pixels.data(),
						l.pixels.size() * sizeof(Color),
						&compressedSize);

			file << (int)opacity << " "
				<< blendMode << " "
				<< compressedSize << "\n";

			file.write((char*)compressedData, compressedSize);

			file << "\n";

			MemFree(compressedData);
		}

	}

	if (ctrl && shift) {
		size_t otherLayer = selectedLayer;
		bool isSwap = false;

		if (IsKeyPressed(KEY_W)) {
			otherLayer = (selectedLayer + 1) % layers.size();
			isSwap = true;
		} 
		else if (IsKeyPressed(KEY_S)) {
			otherLayer = (selectedLayer == 0)
				? layers.size() - 1
				: selectedLayer - 1;
			isSwap = true;
		}
		if (isSwap) {
			std::swap(layers[selectedLayer].pixels,
					  layers[otherLayer].pixels);
			selectedLayer = otherLayer;

			UpdateTexture(layers[selectedLayer].tex,
						  layers[selectedLayer].pixels.data());
			UpdateTexture(layers[otherLayer].tex,
						  layers[otherLayer].pixels.data());
		}
	}
	if(shift){
		if(IsKeyPressed(KEY_Z)){
			if (!redo.empty()) {
				size_t layerIdx = redo.front().first;
				
				undo.push_front({layerIdx, layers[layerIdx].pixels});
				layers[layerIdx].pixels = redo.front().second;
				redo.pop_front();

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
					size_t layerIdx = undo.front().first;
					
					redo.push_front({layerIdx, layers[layerIdx].pixels});

					layers[layerIdx].pixels = undo.front().second;
					undo.pop_front();

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

		if(IsKeyPressed(KEY_D)){
			layers[selectedLayer].blendingMode = (BlendMode)((layers[selectedLayer].blendingMode + 1) % 3);
		}
		if(IsKeyPressed(KEY_A)){
			if(layers[selectedLayer].blendingMode == 0)
				layers[selectedLayer].blendingMode = (BlendMode)(2);
			else
				layers[selectedLayer].blendingMode = (BlendMode)(layers[selectedLayer].blendingMode - 1);
		}

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
		undo.push_front({selectedLayer, getCurrentLayer().pixels});
		while(!redo.empty())
			redo.pop_back();
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

	if(mouseState != IDLE){
		//for(auto& l : layers) {
		//	UpdateTexture(l.tex, l.pixels.data());
		//}
		UpdateTexture(layers[selectedLayer].tex, layers[selectedLayer].pixels.data());
	}

}

void Canvas::Render() {
	for(auto& l : layers) {
		BeginBlendMode(l.blendingMode);
		DrawTexture(l.tex, 0, 0, WHITE);
		EndBlendMode();
    }
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
		char blend = 'A';
		switch(layers[i].blendingMode){
			case BLEND_ADDITIVE:
				blend = 'A';
				break;
			case BLEND_MULTIPLIED:
				blend = 'M';
				break;
			default:
				blend = 'N';
				break;
		}
		DrawText(TextFormat("[%c] Layer: %d", blend , i), 20, 80+(20*y), 20, i == selectedLayer ? GREEN : BLACK);
	}
	DrawText((isBrush ? "Current Mode: BRUSH" : "Current Mode: ERASER"), 20, GetScreenHeight()-60.0f, 20, BLACK);
	DrawText(TextFormat("Transparency: %.0f", ((float)clr.a/255.0f)*100.0f), 20, GetScreenHeight()-40.0f, 20, BLACK);
}

