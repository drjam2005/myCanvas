#include <raylib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <cstring>
#include <canvas.h>
#include <SDLHandler.h>

int main(int argc, char** argv){
	std::string fileName = "";
    int width = 800;
    int height = 600;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            fileName = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            width = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            height = atoi(argv[i + 1]);
            i++;
        } else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage:\n");
            printf("    ./myCanvas\n");
            printf("    ./myCanvas -w <width> -h <height>\n");
            printf("    ./myCanvas -f <fileName>\n");
			return 0;
        } else {
            printf("Unknown argument: %s\n", argv[i]);
        }
    }

	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE );
	InitWindow(width, height, "myCanvas");
	InitSDLTabletInput(GetWindowHandle());
	HideCursor();

	Canvas canvas;
	canvas = Canvas(width, height, 16, fileName);
	//SetExitKey(KEY_NULL);
	
	while(!WindowShouldClose()){
		PumpSDLTabletInput();
		canvas.Update();

		BeginDrawing();
		ClearBackground(DARKGRAY);
		DrawFPS(20, 20);

		canvas.Render();

		EndDrawing();
	}
	ShutdownSDLTabletInput();
	CloseWindow();
	return 0;
}
