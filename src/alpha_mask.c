#if defined(_WIN32)
#include "../vendor/raylib/windows/include/raylib.h"
#include "../vendor/raylib/windows/include/raymath.h"
#elif defined(__unix__) || defined(__APPLE__)
#include "../vendor/raylib/unix/include/raylib.h"
#include "../vendor/raylib/unix/include/raymath.h"
#endif
#include "../vendor/raylib/unix/include/rlgl.h"


int main(void) {
    InitWindow(800, 450, "RenderTexture Alpha Masking");

    Image img = GenImageGradientLinear(800, 600, 90 , GREEN, BLUE);        // Generate image: linear gradient, direction in degrees [0..360], 0=Vertical gradient
    Texture2D background = LoadTextureFromImage(img);
    RenderTexture2D target = LoadRenderTexture(800, 450);

    while (!WindowShouldClose()) {
        // 1. Draw the mask into the RenderTexture
        BeginTextureMode(target);
            ClearBackground(BLANK);
            // Draw white shapes where you want the texture to be visible
            DrawCircleV(GetMousePosition(), 100, GetColor(0xFFFFFFAA));
        EndTextureMode();

        // 2. Multiply the content texture by the destination alpha
        BeginTextureMode(target);
            // Source color is multiplied by destination alpha; destination alpha is preserved
            rlSetBlendFactors(RL_DST_ALPHA, RL_ZERO, RL_FUNC_ADD);
            BeginBlendMode(BLEND_CUSTOM);
                DrawTexture(background, 0, 0, WHITE);
            EndBlendMode();
        EndTextureMode();

        // 3. Draw the final masked RenderTexture to the screen
        BeginDrawing();
            ClearBackground(DARKGRAY);
            // Note: RenderTextures are Y-flipped in OpenGL, so height is negative
            DrawTextureRec(target.texture, 
                (Rectangle){ 0, 0, (float)target.texture.width, (float)-target.texture.height }, 
                (Vector2){ 0, 0 }, WHITE);
        EndDrawing();
    }

    UnloadRenderTexture(target);
    UnloadTexture(background);
    CloseWindow();
    return 0;
}


