#pragma once

#include "../Model/GameModel.hpp"
#include "../View/GameView.hpp"

// Controller: converts player input into model changes and runs the frame loop.
class GameController {
public:
    int run();

private:
    void readInput();
    game::Game model_;
    GameView view_;
};
