#include <algorithm>

#include "canvas.h"
#include "helpers.h"
#include "raylib.h"
#include "raymath.h"

#include "SDLHandler.h"

bool Canvas::handle_pen_events()
{
    PumpSDLTabletInput();

    penPressedThisFrame  = ConsumeTabletPenPressed();
    penReleasedThisFrame = ConsumeTabletPenReleased();

    pressure = 1.0f;
    if (IsTabletPenDown()) 
		GetLatestTabletPressure(&pressure);

#if defined(WIN32)
	if (IsTabletInProximity())
		GetLatestTabletPosition(&pointerPos.x, &pointerPos.y);
#endif

    pointerPressed  = penPressedThisFrame  || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    pointerDown     = IsTabletPenDown()    || IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    pointerReleased = penReleasedThisFrame || IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    bool handled = pointerPressed || pointerDown || pointerReleased || isPenInProximity;
    return handled;
}

bool Canvas::handle_key_events(){
	bool ctrl  = IsKeyDown(KEY_LEFT_CONTROL);
	bool shift = IsKeyDown(KEY_LEFT_SHIFT);
	bool space = IsKeyDown(KEY_SPACE);
	bool alt   = IsKeyDown(KEY_LEFT_ALT);

	if(IsKeyPressed(KEY_TAB))
		isUiHidden = !isUiHidden;

	if (space && !ctrl && !shift) {
		if (pointerPressed) {
			prevMousePos = GetMousePos();
		}
		if (pointerDown) {
			Vector2 mouseDelta = Vector2Subtract(GetMousePos(), prevMousePos);

			Vector2 rotatedDelta = Vector2Rotate(mouseDelta, -rotation);
			canvasPos.x += rotatedDelta.x;
			canvasPos.y += rotatedDelta.y;
			prevMousePos = GetMousePos();
		}
		return true;
	}

	if (ctrl && space) {
		if (pointerDown) {
			static float zoomAccumulator = 0.0f;
			if (ctrl && space && pointerDown) {
				float yDelta = GetMousePos().y - prevMousePos.y;

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
						(GetMousePos().x - canvasPos.x) / oldScale,
						(GetMousePos().y - canvasPos.y) / oldScale
					};

					canvasPos.x = GetMousePos().x - before.x * scale;
					canvasPos.y = GetMousePos().y - before.y * scale;
				}

				prevMousePos = GetMousePos();
			}
		}
	}


	if (ctrl && shift) {
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
		return true;
	}
	if(shift && !ctrl && !space){
		static bool resizingBrush = false;
		static float lastResizeMouseX = 0.0f;

		if(IsKeyPressed(KEY_ENTER)){
			SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
			save();
			save_to_png();
		}

		if(pointerDown){
			if (!resizingBrush || pointerPressed) {
				lastResizeMouseX = GetMousePos().x;
				resizingBrush = true;
			}

			float resizeScale = std::clamp(scale, 0.2f, 7.0f);
			float diff = 0.25f*(GetMousePos().x - lastResizeMouseX)/resizeScale;
			lastResizeMouseX = GetMousePos().x;

			if(isBrush)
				brushSize = fmax(0.5f, fmin(brushSize + diff, 50.0f));
			else
				eraserSize = fmax(0.5f, fmin(eraserSize + diff, 50.0f));
		} else {
			resizingBrush = false;
		}
		return true;
	}
	if (shift && space && !ctrl) {
		static bool rotatingCanvas = false;
		static float lastRotationAngle = 0.0f;

		if (pointerDown) {
			float currentAngle = AngleFromScreenCenter(GetMousePos());

			if (!rotatingCanvas || pointerPressed) {
				lastRotationAngle = currentAngle;
				rotatingCanvas = true;
			} else {
				rotation += NormalizeAngleDelta(currentAngle - lastRotationAngle);
				lastRotationAngle = currentAngle;
			}
		} else {
			rotatingCanvas = false;
		}

		return true;
	}

	if(IsKeyPressed(KEY_LEFT_ALT)){
		currentLayerCache = LoadImageFromTexture(get_current_layer().tex.texture);
		isColorPicking = true;
	}
	if (alt && !ctrl && !space && !shift) {
		Color temp = pick_color(GetMousePos());
		if (temp.a != 0)
			previewClr = temp;

		if (pointerReleased) {
			clr = previewClr;
		}
		return true;
	}

	if(IsKeyReleased(KEY_LEFT_ALT)){
		isColorPicking = false;
		UnloadImage(currentLayerCache);
	}

	if (!IsKeyDown(KEY_LEFT_ALT)) {
		previewClr = clr;
	}

	if(ctrl && !shift){
		if(IsKeyPressed(KEY_E)){
			create_layer(false);
			return true;
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
			return true;
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

		return true;
	}

	return false;
}

bool Canvas::handle_tool_input(){

	bool ctrl  = IsKeyDown(KEY_LEFT_CONTROL);
	bool shift = IsKeyDown(KEY_LEFT_SHIFT);
	bool space = IsKeyDown(KEY_SPACE);
	bool handled = false;

	if(!ctrl && !shift && !space){
		if(IsKeyPressed(KEY_M)){
			isMirror = !isMirror;
			rotation *= -1;
			handled = true;
		}
		if(IsKeyPressed(KEY_FIVE)){
			rotation = 0.0f;
			handled = true;
		}
		if(IsKeyPressed(KEY_A)){
			layers[selectedLayer].opacity = (unsigned char)fmax(layers[selectedLayer].opacity - (255.0f/10.0f), 0.0f);
			handled = true;
		}else if (IsKeyPressed(KEY_D)){
			layers[selectedLayer].opacity = (unsigned char)fmin(layers[selectedLayer].opacity + (255.0f/10.0f), 255.0f);
			handled = true;
		}
		if(IsKeyPressed(KEY_E)){
			isBrush = !isBrush;
			handled = true;
		}

		for(int i = 0; i < 10; ++i){
			if(IsKeyPressed(KEY_ONE+i) && i+1 <= colorQueue.size()){
				clr = colorQueue[i];
				handled = true;
			}
		}


		if (IsKeyPressed(KEY_ENTER)) {
			SetWindowTitle(TextFormat("myCanvas | %s", fileName.c_str()));
			save();
			handled = true;
		}
		if (pointerPressed) {
			SetWindowTitle(TextFormat("myCanvas | %s (*)", fileName.c_str()));
			if(CheckCollisionPointRec(GetMousePos(), colorPickerBounds)){
				isColorPicking = true;
				return true;
			}
			float cWidth = colorPickerRec.width / 6.0f;
			float padding = cWidth / 5.0f;
			size_t cols = 5;
			for (size_t i = 0; i < colorQueue.size(); ++i) {
				Color c = colorQueue[i];
				size_t row = i / cols;
				size_t col = i % cols;

				float xOffset = col * (cWidth + padding);
				float yOffset = row * (cWidth + padding);

				Rectangle bounds = {
					colorPickerRec.x + xOffset,
					colorPickerRec.y + colorPickerRec.height + yOffset + padding,
					cWidth,
					cWidth,
				};
				if(CheckCollisionPointRec(GetMousePos(), bounds)){
					clr = c;
					handled = true;
				}

			}

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
			handled = true;
		}
		if(pointerDown) {
			if(isColorPicking)
				return true;

			Vector2 currentCanvasMouse = screen_to_canvas(GetMousePos());
			Vector2 prevCanvasMouse = screen_to_canvas(prevMousePos);

			if(mouseState == HELD && prevMousePos.x >= 0) {
				draw_line(prevCanvasMouse, currentCanvasMouse);
			}
			mouseState = HELD;
			handled = true;
		} 
		if(pointerReleased) {
			if(isColorPicking){
				if(!contains(colorQueue, clr)){
					colorQueue.push_front(clr);
					if(colorQueue.size() > 10){
						colorQueue.pop_back();
					}
				}
			}
			isColorPicking = false;
			mouseState = IDLE;
			prevMousePos = {-1,-1};
			handled = true;
		}
	}

	return handled;
}
