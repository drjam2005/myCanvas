#include <fstream>
#include <vector>

#include "events.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

#include "canvas.h"

// misc
void Canvas::save(){
	if (fileName == "") fileName = "myTemp";

	std::ofstream file(fileName, std::ios::binary);
	if (!file.is_open()) return;

	file << width << " " << height << " " << layers.size() << "\n";
	// save colors too
	file << colorQueue.size() << '\n';
	for(auto c : colorQueue){
		file << (int)c.r << " " << (int)c.g << " " << (int)c.b << '\n';
	}
	file << '\n';
	for (auto& l : layers) {
		int compressedSize = 0;

		unsigned char opacity = (unsigned char)l.opacity;
		int blendMode = (int)l.blendingMode;

		Image img = LoadImageFromTexture(l.tex.texture);
		Color* colors = LoadImageColors(img);
		unsigned char* compressedData =
			CompressData((unsigned char*)colors,
					(img.width * img.height) * sizeof(Color),
					&compressedSize);

		file << (int)opacity << " "
			<< blendMode << " "
			<< compressedSize << "\n";

		file.write((char*)compressedData, compressedSize);

		file << "\n";

		MemFree(compressedData);
		UnloadImageColors(colors);
		UnloadImage(img);
	}

	bus.pushEvent((Event){
		.type = EVENT_NOTIFY,
		.notify_message = TextFormat("Saved %s!", fileName.c_str())
	});

}

void Canvas::save_to_png(){
	RenderTexture finalTex = LoadRenderTexture(width, height);
	BeginTextureMode(finalTex);
	ClearBackground(BLANK);

	for(auto& l : layers) {
		BeginBlendMode(l.blendingMode);

		Rectangle source = { 0, 0, (float)width, (float)height};
		Rectangle dest = { 0, 0,   (float)width,  (float)height};

		DrawTexturePro(l.tex.texture, source, dest, (Vector2){0, 0}, 0.0f, Color{255,255,255,(unsigned char)l.opacity});

		EndBlendMode();
	}

	EndTextureMode();
	Image finalImage = LoadImageFromTexture(finalTex.texture);
	std::string finalFilePath = fileName + ".png";
	ExportImage(finalImage, finalFilePath.c_str());

	bus.pushEvent((Event){
		.type = EVENT_NOTIFY,
		.notify_message = TextFormat("Saved to %s", finalFilePath.c_str())
	});

	UnloadImage(finalImage);
	UnloadRenderTexture(finalTex);
}

bool Canvas::load(std::string fileName) {
	if(fileName.empty())
		return false;

	std::ifstream file(fileName, std::ios::binary);
	if (file.is_open()) {
		int w, h, layerCount;
		std::string header;
		if (!std::getline(file, header)){} else
		{
			if (sscanf(header.c_str(), "%d %d %d", &w, &h, &layerCount) == 3) {
				this->width = w;
				this->height = h;
				SetWindowSize(w, h);
				int colorCount;
				std::string clrc;
				std::getline(file, clrc);
				sscanf(clrc.c_str(), "%d", &colorCount);
				for(size_t i = 0; i < colorCount; ++i){
					std::string meta;
					if (!std::getline(file, meta)) break;

					int red, green, blue;
					sscanf(meta.c_str(), "%d %d %d", &red, &green, &blue);
					Color clr = {(unsigned char)red, (unsigned char)green, (unsigned char)blue, 255};
					colorQueue.push_back(clr);
				}
				file.ignore(1, '\n'); 
				

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
						create_layer(false);
						Layer& l = layers.back();
						l.opacity = (unsigned char)opacityInt;
						l.blendingMode = (BlendMode)blendMode;

						UpdateTexture(l.tex.texture, decompressed);
						MemFree(decompressed);
					}
				}
				return true;
			}
		}
	}
	return false;
}

void Canvas::handle_file_loading(){
	if (load(fileName)) {
		selectedLayer = layers.size() - 1;
		clr = colorQueue[0];
		SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
	} else {
		layers.clear(); 

		create_layer(true);  
		create_layer(false); 

		selectedLayer = 1;
		colorQueue.push_front(BLACK);
		SetWindowTitle("myCanvas | [NEW]");
	}
	if(colorQueue.size() <= 0)
		colorQueue.push_front(BLACK);
	else
		clr = colorQueue[0];

}

void Canvas::handle_window(){
	pressure = 1.0f;
	canvasDimensions.x = 0;
	canvasDimensions.y = 0;
	canvasDimensions.width = width;
	canvasDimensions.height = height;

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
