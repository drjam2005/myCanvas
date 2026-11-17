#include "../vendor/raylib/unix/include/raylib.h"
#include "../vendor/raylib/unix/include/raymath.h"

typedef struct {
	bool isPanning;
	bool mousePressed;
	bool mouseDown;
	bool mouseReleased;
	Vector2 mousePos;
	Vector2 prevPos;
} editorState;

int main(void) {
    InitWindow(800, 600, "myCanvas");

    RenderTexture2D canvas = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
	editorState state = {0};
	Vector2 origin = {0, 0};
    while (!WindowShouldClose()) {
		// check state
        state.mousePos = GetMousePosition();
		state.mouseDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
		state.mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
		state.mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
		state.isPanning = state.mouseDown && IsKeyDown(KEY_SPACE);

		if(state.isPanning)
			origin = Vector2Subtract(origin, Vector2Subtract(state.mousePos, state.prevPos));

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(
            canvas.texture,
            (Rectangle){0, 0, canvas.texture.width, -canvas.texture.height},
            (Rectangle){0, 0, canvas.texture.width, canvas.texture.height},
            origin,
            0.0f,
            WHITE
        );

        BeginTextureMode(canvas);
        if (state.mousePressed)
            state.prevPos = state.mousePos;
        if (state.mouseDown && !state.isPanning) {
            DrawLineEx(Vector2Add(origin, state.prevPos), Vector2Add(origin, state.mousePos), 20.0f, BLACK);
            DrawCircleV(Vector2Add(origin, state.mousePos), 10.0f, BLACK);
            state.prevPos = state.mousePos;
        }

		state.prevPos = state.mousePos;
        EndTextureMode();
		DrawFPS(20, 20);

        EndDrawing();
    }

    UnloadRenderTexture(canvas);
    CloseWindow();
}
