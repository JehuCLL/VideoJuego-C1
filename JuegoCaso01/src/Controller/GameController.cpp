#include "GameController.hpp"
#include "raylib.h"

void GameController::readInput() {
    model_.input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    model_.input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    model_.input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    model_.input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    model_.input.firePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    model_.input.mouse = {GetMousePosition().x, GetMousePosition().y};
    if (IsKeyPressed(KEY_R)) model_.reset();
}

int GameController::run() {
    InitWindow(game::Width, game::Height, "Caso 1 - Exploracion y combate 2D");
    SetTargetFPS(60);
    HideCursor();
    view_.load();

    while (!WindowShouldClose()) {
        const bool focused = IsWindowFocused();
        if (focused) {
            readInput();
            model_.update(GetFrameTime());
        } else {
            model_.input.clearKeys();
        }
        BeginDrawing();
        view_.draw(model_, focused);
        EndDrawing();
    }

    view_.unload();
    CloseWindow();
    return 0;
}
