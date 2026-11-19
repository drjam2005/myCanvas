#if defined(_WIN32)
#include "../vendor/raylib/windows/include/raylib.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../vendor/raylib/unix/include/raylib.h"
#endif
#include <stdio.h>

#include "../include/plug/plug.h"

int main(void) {
    InitWindow(800, 600, "myCanvas");
	Plug plug = {0};

	if(!reloadPlug(&plug) || !plug.plug_init()) {
		fprintf(stderr, "Could not initialize the canvas plugin.\n");
		CloseWindow();
		return 1;
	}

    while (!WindowShouldClose()) {

		if(IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_CONTROL)) {
#ifdef RELOAD
			void* data = plug.plug_pre_reload();
			if(!reloadPlug(&plug))
				return 1;
			plug.plug_post_reload(data);
			printf("reloading!\n");
#else
			printf("static!\n");
#endif
		}

		plug.plug_update();

        BeginDrawing();
        ClearBackground(RAYWHITE);

		plug.plug_render();

		DrawFPS(20, 20);
        EndDrawing();
    }

    CloseWindow();
}
