#include <cstdio>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "canvas.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

// helper
void DrawTextContrast(const char *text, int posX, int posY, int fontSize, Color color) {
	Color darkDARKGRAY{ 40, 40, 40, 255 };
    DrawText(text, posX + 1, posY + 1, fontSize, darkDARKGRAY);
    DrawText(text, posX - 1, posY - 1, fontSize, darkDARKGRAY);
    DrawText(text, posX + 1, posY - 1, fontSize, darkDARKGRAY);
    DrawText(text, posX - 1, posY + 1, fontSize, darkDARKGRAY);

    DrawText(text, posX, posY, fontSize, color);
}

Canvas::Canvas(int width, int height, size_t maxLayers, std::string fileName)
    : width(width), height(height),
      brushSize(20.0f), eraserSize(20.0f), selectedLayer(0),
      mouseState(IDLE), prevMousePos({-1,-1}), transparency(255),
      fileName(fileName), clr(BLACK), isBrush(true), scale(1.0f), canvasDimensions((Rectangle){0, 0, (float)width, (float)height})
{
	bool loadedSuccessfully = false;

	if (fileName != "") {
		std::ifstream file(fileName, std::ios::binary);
		if (file.is_open()) {
			int w, h, layerCount;
			std::string header;
			if (!std::getline(file, header)){} else
			{
				if (sscanf(header.c_str(), "%d %d %d", &w, &h, &layerCount) == 3) {
					this->width = w;
					this->height = h;

					for (int i = 0; i < layerCount; i++) {
						std::string meta;
						if (!std::getline(file, meta)) break;

						int opacityInt, blendMode, compressedSize;
						sscanf(meta.c_str(), "%d %d %d", &opacityInt, &blendMode, &compressedSize);

						std::vector<unsigned char> compressedBuffer(compressedSize);
						file.read((char*)compressedBuffer.data(), compressedSize);

						file.ignore(1, '\n'); 

						int decompressedSize = 0;
						unsigned char* decompressed = DecompressData(compressedBuffer.data(), compressedSize, &decompressedSize);

						if (decompressed) {
							createLayer(false);
							Layer& l = layers.back();
							l.opacity = (unsigned char)opacityInt;
							l.blendingMode = (BlendMode)blendMode;

							UpdateTexture(l.tex.texture, decompressed);
							MemFree(decompressed);
						}
					}
					loadedSuccessfully = true;
				}
			}
		}
	}

	if (!loadedSuccessfully) {
		layers.clear(); 
		createLayer(true);  
		createLayer(false); 
		selectedLayer = 1;
		SetWindowTitle("myCanvas | [NEW]");
	} else {
		selectedLayer = layers.size() - 1;
		SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
	}

	colors['1'] = BLACK;
	colors['2'] = RED;
	colors['3'] = GREEN;
	colors['4'] = BLUE;
	colors['5'] = ORANGE;
}

void Canvas::createLayer(bool whiteBackground) {
    layers.emplace_back(width, height, whiteBackground);
}

Layer& Canvas::getCurrentLayer() {
    return layers[selectedLayer];
}

void Canvas::drawCircle(Vector2 v1) {
    float r = isBrush ? brushSize : eraserSize;
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


void Canvas::drawLine(Vector2 from, Vector2 to) {
    float r = isBrush ? brushSize : eraserSize;
    float spacing = r * 0.1f;

    Vector2 dir = Vector2Subtract(to, from);
    float dist = Vector2Length(dir);
    dir = Vector2Normalize(dir);

    BeginTextureMode(layers[selectedLayer].tex);

    if (!isBrush) {
        rlSetBlendFactors(RL_ZERO, RL_ONE_MINUS_SRC_ALPHA, RL_SRC_ALPHA);
        rlSetBlendMode(BLEND_CUSTOM);
        
        for (float d = 0; d <= dist; d += spacing) {
            Vector2 p = Vector2Add(from, Vector2Scale(dir, d));
            Vector2 drawPos = { p.x, (float)GetScreenHeight() - p.y };
            DrawCircleV(drawPos, r, WHITE); 
        }
		DrawLineEx(Vector2{from.x, GetScreenHeight()-from.y}, 
				Vector2{to.x, GetScreenHeight()-to.y}, r, WHITE);
        
        rlSetBlendMode(BLEND_ALPHA);
    } else {
        for (float d = 0; d <= dist; d += spacing) {
            Vector2 p = Vector2Add(from, Vector2Scale(dir, d));
            Vector2 drawPos = { p.x, (float)GetScreenHeight() - p.y };
            DrawCircleV(drawPos, r, clr);
        }
		DrawLineEx(Vector2{from.x, GetScreenHeight()-from.y}, 
				Vector2{to.x, GetScreenHeight()-to.y}, r, clr);
    }

    EndTextureMode();
}

void Canvas::Update() {
    Vector2 mousePos = GetMousePosition();
	bool ctrl  = IsKeyDown(KEY_LEFT_CONTROL);
	bool shift = IsKeyDown(KEY_LEFT_SHIFT);
	bool space = IsKeyDown(KEY_SPACE);
	bool alt   = IsKeyDown(KEY_LEFT_ALT);

	if (space && !ctrl && !shift){
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			float yOffset = mousePos.y - prevMousePos.y;
			float xOffset = mousePos.x - prevMousePos.x;
			canvasPos.y += yOffset;
			canvasPos.x += xOffset;
		}
	}

	if (ctrl && space) {
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			static float zoomAccumulator = 0.0f;
			if (ctrl && space && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
				Vector2 mousePos = GetMousePosition();
				float yDelta = mousePos.y - prevMousePos.y;

				zoomAccumulator += yDelta;

				float PIXELS_PER_STEP = 10.0f;
				float ZOOM_STEP = 1.1f;

				if (fabsf(zoomAccumulator) >= PIXELS_PER_STEP) {
					float oldScale = scale;

					if (zoomAccumulator < 0)
						scale *= ZOOM_STEP;
					else
						scale /= ZOOM_STEP;

					zoomAccumulator = 0.0f;
					scale = std::clamp(scale, 0.05f, 20.0f);

					Vector2 before = {
						(mousePos.x - canvasPos.x) / oldScale,
						(mousePos.y - canvasPos.y) / oldScale
					};

					canvasPos.x = mousePos.x - before.x * scale;
					canvasPos.y = mousePos.y - before.y * scale;
				}

				prevMousePos = mousePos;
			}
		}
	}

	if (IsKeyPressed(KEY_ENTER)) {
		if (fileName == "") return;

		std::ofstream file(fileName, std::ios::binary);
		if (!file.is_open()) return;

		file << width << " " << height << " " << layers.size() << "\n";
		for (auto& l : layers) {
			int compressedSize = 0;

			unsigned char opacity = (unsigned char)l.opacity;
			int blendMode = (int)l.blendingMode;

			Image img = LoadImageFromTexture(l.tex.texture);
			unsigned char* compressedData =
				CompressData((unsigned char*)(LoadImageColors(img)),
						(img.width * img.height) * sizeof(Color),
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
		// swapping
		{
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
				std::swap(layers[selectedLayer].tex, layers[otherLayer].tex);
				
				std::swap(layers[selectedLayer].blendingMode, layers[otherLayer].blendingMode);
				std::swap(layers[selectedLayer].opacity, layers[otherLayer].opacity);
				
				std::swap(layers[selectedLayer].width, layers[otherLayer].width);
				std::swap(layers[selectedLayer].height, layers[otherLayer].height);

				selectedLayer = otherLayer;
			}
		}
		if (IsKeyPressed(KEY_Z)) {
			if (!redo.empty()) {
				size_t layerIdx = redo.front().first;
				Image redoImage = redo.front().second;

				Image current = LoadImageFromTexture(layers[layerIdx].tex.texture);
				undo.push_front({layerIdx, current});

				UpdateTexture(layers[layerIdx].tex.texture, redoImage.data);

				UnloadImage(redoImage);
				redo.pop_front();
			}
		}
	}
	if(shift && !ctrl && !space){
		if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
			// nothing yet...
			prevMousePos = GetMousePosition();
		}

		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
			Vector2 currPos = GetMousePosition();
			float diff = 0.25f*(currPos.x - prevMousePos.x);
			prevMousePos = currPos;
			if(isBrush)
				brushSize = fmax(0.5f, fmin(brushSize + diff, 50.0f));
			else
				eraserSize = fmax(0.5f, fmin(eraserSize + diff, 50.0f));
		}
		return;
	}

	if(ctrl && !shift){
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
				Image prevImage = undo.front().second;

				Image current = LoadImageFromTexture(layers[layerIdx].tex.texture);
				redo.push_front({layerIdx, current});

				UpdateTexture(layers[layerIdx].tex.texture, prevImage.data);

				UnloadImage(prevImage); 
				undo.pop_front();
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
		isBrush = !isBrush;
	}

	for(auto c : colors){
		if(IsKeyPressed(c.first)){
			clr = c.second;
			clr.a = transparency;
		}
	}

	if(!ctrl && !shift && !space){
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Image snapshot = LoadImageFromTexture(layers[selectedLayer].tex.texture);
			undo.push_front({selectedLayer, snapshot});

			if (undo.size() > 10) {
				UnloadImage(undo.back().second);
				undo.pop_back();
			}
			
			while (!redo.empty()) {
				UnloadImage(redo.back().second);
				redo.pop_back();
			}
		}
		// Inside Update(), replace the mouse button down block:
		if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			// Transform Screen Space -> Canvas Space
			Vector2 currentCanvasMouse = {
				(mousePos.x - canvasPos.x) / scale,
				(mousePos.y - canvasPos.y) / scale
			};
			
			Vector2 prevCanvasMouse = {
				(prevMousePos.x - canvasPos.x) / scale,
				(prevMousePos.y - canvasPos.y) / scale
			};

			if(mouseState == HELD && prevMousePos.x >= 0) {
				drawCircle(currentCanvasMouse);
				drawLine(prevCanvasMouse, currentCanvasMouse);
			} else {
				drawCircle(currentCanvasMouse);
			}
			mouseState = HELD;
		} else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			mouseState = IDLE;
			prevMousePos = {-1,-1};
		}
	   
	}
	prevMousePos = mousePos;
}

void Canvas::Render() {
	for(auto& l : layers) {
        BeginBlendMode(l.blendingMode);
        
        Rectangle source = { 0, 0, (float)l.tex.texture.width, (float)l.tex.texture.height };
        Rectangle dest = { canvasPos.x, canvasPos.y, (float)width * scale, (float)height * scale };
        
        DrawTexturePro(l.tex.texture, source, dest, (Vector2){0, 0}, 0.0f, Color{255,255,255,(unsigned char)l.opacity});
        
        EndBlendMode();
    }
	// draw colors
	DrawTextContrast("Color Bindings: ", 20, 20, 20, WHITE);
	size_t x = colors.size();
	for(auto c : colors){
		DrawTextContrast(TextFormat("%d", c.first-'0'), 20+(20*x), 40, 20, c.second);
		if(c.second.r == clr.r &&
		   c.second.g == clr.g &&
		   c.second.b == clr.b)
			DrawTextContrast("_", 20+(20*x), 45, 20, BLACK);
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
		DrawTextContrast(TextFormat("[%c] Layer: %d", blend , i), 20, 80+(20*y), 20, i == selectedLayer ? GREEN : WHITE);
	}
	DrawTextContrast((isBrush ? "Current Mode: BRUSH" : "Current Mode: ERASER"), 20, GetScreenHeight()-60.0f, 20, WHITE);
	DrawTextContrast(TextFormat("Transparency: %.0f", ((float)clr.a/255.0f)*100.0f), 20, GetScreenHeight()-40.0f, 20, WHITE);
	BeginBlendMode(BLEND_SUBTRACT_COLORS);
	DrawCircleLinesV(GetMousePosition(), (scale)*(isBrush ? brushSize : eraserSize), WHITE);
	DrawCircleLinesV(GetMousePosition(), (scale)*(isBrush ? brushSize : eraserSize) - 1.0f, BLACK);
	EndBlendMode();
}

