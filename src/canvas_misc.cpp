#include <iostream>
#include <fstream>
#include <vector>

#include "canvas.h"
#include "raylib.h"

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
	std::cout << "Saved " << fileName << "!" << '\n';
}

void Canvas::save_to_png(){
	RenderTexture finalTex = LoadRenderTexture(width, height);
	BeginTextureMode(finalTex);
	ClearBackground(BLANK);
	for(auto& l : layers) {
		BeginBlendMode(l.blendingMode);

		Rectangle source = { 0, 0, (float)width, -(float)height};
		Rectangle dest = { 0, 0,   (float)width,  (float)height};

		DrawTexturePro(l.tex.texture, source, dest, (Vector2){0, 0}, 0.0f, Color{255,255,255,(unsigned char)l.opacity});

		EndBlendMode();
	}
	EndTextureMode();
	Image finalImage = LoadImageFromTexture(finalTex.texture);
	std::string finalFilePath = fileName + ".png";
	ExportImage(finalImage, finalFilePath.c_str());
	std::cout << "Saved: " << finalFilePath << '\n';

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
