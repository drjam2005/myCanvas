#include <raylib.h>
#include <canvas.h>

#define WIDTH 640
#define HEIGHT 480

int main(){
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "myCanvas");
	HideCursor();
	Canvas canvas(WIDTH, HEIGHT, 16);
	while(!WindowShouldClose()){
		canvas.Update();

		BeginDrawing();
		ClearBackground(WHITE);

		canvas.Render();

		DrawFPS(GetScreenWidth()-120, 20);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
