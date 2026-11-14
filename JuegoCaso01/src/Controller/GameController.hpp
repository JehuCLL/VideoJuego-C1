#pragma once

#include "../Model/GameModel.hpp"
#include "../View/GameView.hpp"
#include <array>
#include <chrono>

// Controller: converts player input into model changes and runs the frame loop.
class GameController {
public:
    int run();

private:
    bool readInput();
    game::Game model_;
    GameView view_;
    std::array<float,100> latencySamples_{};
    unsigned sampleCount_=0;
    float p95LatencyMs_=0;
};
