#include <raylib.h>
#include <canvas.h>

#define WIDTH 600
#define HEIGHT 900

int main(int argc, char** argv){
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(WIDTH, HEIGHT, "myCanvas");
	HideCursor();

	Canvas canvas; std::string fileName = "";
	if(argc == 2)
		fileName = argv[1];
	canvas = Canvas(WIDTH, HEIGHT, 16, fileName);
	
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
