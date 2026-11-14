#include "GameController.hpp"
#include "raylib.h"
#include <algorithm>

bool GameController::readInput() {
    const auto mouse=GetMousePosition();
    bool action=IsKeyPressed(KEY_W)||IsKeyPressed(KEY_A)||IsKeyPressed(KEY_S)||IsKeyPressed(KEY_D)
        ||IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_RIGHT)
        ||IsMouseButtonPressed(MOUSE_BUTTON_LEFT)||mouse.x!=model_.input.mouse.x||mouse.y!=model_.input.mouse.y;
    model_.input.up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    model_.input.down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    model_.input.left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    model_.input.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    model_.input.firePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    model_.input.mouse = {mouse.x,mouse.y};
    if (IsKeyPressed(KEY_R)) model_.reset();
    return action;
}

int GameController::run() {
    InitWindow(game::Width, game::Height, "Caso 1 - Exploracion y combate 2D");
    SetTargetFPS(60);
    HideCursor();
    view_.load();

    while (!WindowShouldClose()) {
        const bool focused = IsWindowFocused();
        const auto inputTime=std::chrono::steady_clock::now();
        bool action=false;
        if (focused) {
            action=readInput();
            model_.update(GetFrameTime());
        } else {
            model_.input.clearKeys();
        }
        BeginDrawing();
        view_.draw(model_, focused, p95LatencyMs_, sampleCount_);
        EndDrawing();
        if(action){
            const auto drawn=std::chrono::steady_clock::now();
            float ms=std::chrono::duration<float,std::milli>(drawn-inputTime).count();
            latencySamples_[sampleCount_%latencySamples_.size()]=ms;
            ++sampleCount_;
            auto ordered=latencySamples_;
            auto n=std::min<std::size_t>(sampleCount_,ordered.size());
            std::sort(ordered.begin(),ordered.begin()+n);
            p95LatencyMs_=ordered[(95*n+99)/100-1];
        }
    }

    view_.unload();
    CloseWindow();
    return 0;
}
