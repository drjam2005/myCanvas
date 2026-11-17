#include "../vendor/raylib/unix/include/raylib.h"

int main(void) {
    InitWindow(800, 600, "myCanvas");
    //SetTargetFPS(60);

    RenderTexture2D canvas = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    Vector2 prevMousePos = {0.0f, 0.0f};
    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(
            canvas.texture,
            (Rectangle){0, 0, canvas.texture.width, -canvas.texture.height},
            (Rectangle){0, 0, canvas.texture.width, canvas.texture.height},
            (Vector2){0, 0},
            0.0f,
            WHITE
        );

        BeginTextureMode(canvas);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            prevMousePos = mousePos;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            DrawLineEx(prevMousePos, mousePos, 20.0f, BLACK);
            DrawCircleV(mousePos, 10.0f, BLACK);
            prevMousePos = mousePos;
        }

        EndTextureMode();
		DrawFPS(20, 20);

        EndDrawing();
    }

    UnloadRenderTexture(canvas);
    CloseWindow();
}
