#include "raylib.h"
#include <cmath>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "rlgl.h"

#include "canvas.h"
#include "helpers.h"

void Canvas::render_layers(){
	Vector2 screenCenter = { (float)GetScreenWidth() * 0.5f, (float)GetScreenHeight() * 0.5f };
    for(auto& l : layers) {
        BeginBlendMode(l.blendingMode);

        Rectangle source = { 0, 0, (float)width, -(float)height };
        if (isMirror) source.width *= -1;

        Rectangle dest = {
            screenCenter.x,
            screenCenter.y,
            (float)width * scale,
            (float)height * scale
        };

        Vector2 origin = {
            screenCenter.x - canvasPos.x,
            screenCenter.y - canvasPos.y
        };

        DrawTexturePro(
            l.tex.texture,
            source,
            dest,
            origin,
            rotation * RAD2DEG,
            Color{255, 255, 255, (unsigned char)l.opacity}
        );

        EndBlendMode();
    }
}

void Canvas::render_color_picker(){

	GuiColorPicker(colorPickerRec, "Colors", &clr);

	float cWidth = colorPickerRec.width / 6.0f;
	float padding = cWidth / 5.0f;
	size_t cols = 5;
	for (size_t i = 0; i < colorQueue.size(); ++i) {
		Color c = colorQueue[i];
		size_t row = i / cols;
		size_t col = i % cols;

		float xOffset = col * (cWidth + padding);
		float yOffset = row * (cWidth + padding);

		DrawRectangle(
			colorPickerRec.x + xOffset,
			colorPickerRec.y + colorPickerRec.height + yOffset + padding,
			cWidth,
			cWidth,
			c
		);

		if (c.r == clr.r && c.g == clr.g && c.b == clr.b) {
			Rectangle outside = {
				colorPickerRec.x + xOffset,
				colorPickerRec.y + colorPickerRec.height + yOffset + padding,
				cWidth*0.95f,
				cWidth*0.95f
			};
			DrawRectangleRoundedLinesEx(outside, 0.0f, 20, 5, BLACK);
		}
	}

	BeginBlendMode(BLEND_SUBTRACT_COLORS);
		if(CheckCollisionPointRec(GetMousePos(), colorPickerBounds) || isColorPicking){
			DrawCircleV(GetMousePos(), 1.0f*scale, clr);
			float smaller = fmax(20.0f, fmin(GetScreenWidth(), GetScreenHeight())*0.1f);
			DrawRectangle((int)GetMousePos().x - (smaller/2.0f)-1.0f, (int)GetMousePos().y - (smaller*1.5f)-1.0f, smaller+2.0f, smaller+2.0f, BLACK);
			DrawRectangle((int)GetMousePos().x - (smaller/2.0f), (int)GetMousePos().y - (smaller*1.5f), smaller, smaller, previewClr);
		}else{
			DrawCircleLinesV(GetMousePos(), (scale)*(isBrush ? brushSize : eraserSize), WHITE);
			DrawCircleLinesV(GetMousePos(), (scale)*(isBrush ? brushSize : eraserSize) - 1.0f, BLACK);
		}
	EndBlendMode();
	
	float pickerBaseSize = fmax(500.0f, fmin(GetScreenWidth(), GetScreenHeight()));
	float pickerSize = pickerBaseSize * 0.2f;
	colorPickerRec = {
		GetScreenWidth() - (pickerBaseSize * 0.3f),
		pickerBaseSize * 0.1f,
		pickerSize,
		pickerSize
	};
	colorPickerBounds = colorPickerRec;
	colorPickerBounds.width += 35;

}

void Canvas::render_layer_ui(){
	auto events = bus.getEvents();
	for(auto event : events) {
		if (event.type == EVENT_NOTIFY) {
			messageQueue.push_back(
					(NotifMessage) {
						.message = event.notify_message,
						.lifeTime = 5.0f
					}
				);
		}
	}

	DrawTextContrast("Layers:", 15, 20, 30, WHITE);
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
		DrawTextContrast(TextFormat("[%c] Layer: %d [%.2f%]", blend , i, 100.0f*(layers[i].opacity/255.0f)), 20, 60+(24*y), 20, i == selectedLayer ? GREEN : WHITE);
	}
	DrawTextContrast((isBrush ? "Current Mode: BRUSH" : "Current Mode: ERASER"), 20, GetScreenHeight()-60.0f, 20, isBrush ? WHITE : PINK);
	DrawTextContrast(TextFormat("Transparency: %.0f", ((float)clr.a/255.0f)*100.0f), 20, GetScreenHeight()-40.0f, 20, WHITE);
	if(isMirror){
		DrawTextContrast(TextFormat("Mirrored: True"), 20, GetScreenHeight()-100.0f, 20, GREEN);
	}else
		DrawTextContrast(TextFormat("Mirrored: False"), 20, GetScreenHeight()-100.0f, 20, WHITE);

	// messages drawing
	messageQueue.erase(std::remove_if( 
				messageQueue.begin(), messageQueue.end(), [](NotifMessage& msg) { return msg.lifeTime <= 0.0f; }
		), messageQueue.end());

	for(NotifMessage& message : messageQueue) {
		message.lifeTime -= GetFrameTime();
	}
	
	float notifYPos = 10.0f;
	size_t index = 0;

	for(NotifMessage& message : messageQueue) {

		Vector2 messageDimensions = MeasureTextEx(GetFontDefault(), message.message.c_str(), 30, 5.0f);

		messageDimensions.x = std::max(messageDimensions.x, 50.0f);
		messageDimensions.y += 15;
		Rectangle rec = {
			.x = GetScreenWidth() - messageDimensions.x,
			.y = notifYPos,
			.width = messageDimensions.x,
			.height = messageDimensions.y,
		};
		DrawRectangleRec(rec, GRAY);
		DrawRectangleLinesEx(rec, 5.0, BLACK);
		DrawTextContrast(message.message.c_str(), rec.x+10.0, rec.y+7.5, 30, WHITE);

		notifYPos += messageDimensions.y + 5;
	}
}
