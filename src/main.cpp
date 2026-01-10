#include <raylib.h>

#define WIDTH 900
#define HEIGHT 900

int main(){
	InitWindow(WIDTH, HEIGHT, "myCanvas");
	SetTargetFPS(60);
	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(WHITE);

		DrawText("Hello, Raylib!\n", 20, 20, 40, BLACK);


		EndDrawing();
	}

	CloseWindow();
	return 0;
}
