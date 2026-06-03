#pragma once
#ifndef HELPERS_H
#define HELPERS_H

#include "raylib.h"
#include <deque>

void DrawTextContrast(const char *text, int posX, int posY, int fontSize, Color color);
bool contains(const std::deque<Color>& d, Color value);
float AngleFromScreenCenter(Vector2 pos);
float NormalizeAngleDelta(float delta);

#endif // HELPERS_H
