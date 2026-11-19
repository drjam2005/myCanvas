#if defined(_WIN32)
#include "../vendor/raylib/windows/include/raylib.h"
#include "../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../vendor/raylib/unix/include/raylib.h"
#include "../vendor/raylib/unix/include/raymath.h"
#endif

#include "../include/systems/editor_info.h"
#include "../include/systems/updater.h"
#include "../include/systems/renderer.h"

int main(void) {
    InitWindow(800, 600, "myCanvas");

	editorState state = {0};
	Canvas canvas = {0};
	canvas.dimensions = (Vector2){GetScreenWidth(), GetScreenHeight()};
	create_layer(&canvas);
    while (!WindowShouldClose()) {

		update(&state, &canvas);

        BeginDrawing();
        ClearBackground(RAYWHITE);

		render(&state, &canvas);

		DrawFPS(20, 20);
        EndDrawing();
    }

    CloseWindow();
}
