#pragma once

#include "raylib.h"
#include "../Model/GameModel.hpp"

// View: presents model state without changing gameplay rules.
class GameView {
public:
    void load();
    void unload();
    void draw(const game::Game& model, bool focused) const;

private:
    Texture2D explorer_{};
    Texture2D slime_{};
};
