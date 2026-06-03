#include "helpers.h"
#include <cmath>

// helpers
void DrawTextContrast(const char *text, int posX, int posY, int fontSize, Color color) {
	Color darkDARKGRAY{ 40, 40, 40, 255 };
    DrawText(text, posX + 1, posY + 1, fontSize, darkDARKGRAY);
    DrawText(text, posX - 1, posY - 1, fontSize, darkDARKGRAY);
    DrawText(text, posX + 1, posY - 1, fontSize, darkDARKGRAY);
    DrawText(text, posX - 1, posY + 1, fontSize, darkDARKGRAY);

    DrawText(text, posX, posY, fontSize, color);
}

bool contains(const std::deque<Color>& d, Color value) {
    for (Color v : d) {
        if (v.r == value.r && v.g == value.g && v.b == value.b)
            return true;
    }
    return false;
}

float AngleFromScreenCenter(Vector2 pos) {
	return atan2f(pos.y - (GetScreenHeight()*0.5f), pos.x - (GetScreenWidth()*0.5f));
}

float NormalizeAngleDelta(float delta) {
	while (delta > PI) delta -= 2.0f*PI;
	while (delta < -PI) delta += 2.0f*PI;
	return delta;
}
