#pragma once

#include <chrono>
#include <cmath>
#include <iostream>

/// Keep track of the game state changes.
struct GameStateTracker
{
    enum GameState
    {
        NONE = -1,
        INTRO = 0,
        PLAY,
        HIGHSCORE
    };
    static GameState gameState;

    // A block of 16kb
    static uint16_t memoryCopy[1024 * 16];

    // Instruction executed more often.
    static size_t maxAddress;
    static size_t oldMaxAddress;
    // Instruction's amount of invocations.
    static uint16_t oldMaxValue;
    static std::chrono::time_point<std::chrono::steady_clock> oldTime;
    static std::chrono::time_point<std::chrono::steady_clock>
        highscoreEnterTime;
    static std::chrono::time_point<std::chrono::steady_clock> currentTime;
    static uint16_t invocationCounter;
    static size_t playToIntroCount;
    static size_t playToHighCount;

    static void identifyGameState(uint16_t programCounter);
};