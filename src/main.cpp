#include <raylib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>
#include <cstring>
#include <string>

#include "canvas.h"
#include "SDLHandler.h"

int width = 800;
int height = 600;
std::string fileName = "";

bool handleArgs(int argc, char** argv);

int main(int argc, char** argv){

	if(!handleArgs(argc, argv))
		return 1;

	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE );
	InitWindow(width, height, "myCanvas");

	InitSDLTabletInput();
	//SetTargetFPS(30);

	HideCursor();
	Canvas canvas(width, height, 16, fileName);
	//SetExitKey(KEY_NULL);
		
	while(!WindowShouldClose()){
		PumpSDLTabletInput();
		canvas.Update();

		BeginDrawing();
		ClearBackground(DARKGRAY);

		canvas.Render();

		DrawFPS(20, 20);

		EndDrawing();
	}
	ShutdownSDLTabletInput();
	CloseWindow();
	return 0;
}

bool handleArgs(int argc, char** argv) {
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
			return false;
        } else {
            printf("Unknown argument: %s\n", argv[i]);
        }
    }
	return true;
}
