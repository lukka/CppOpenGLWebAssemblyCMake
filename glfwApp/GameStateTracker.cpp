#include "GameStateTracker.h"
#include <cstring>

GameStateTracker::GameState GameStateTracker::gameState =
    GameStateTracker::NONE;

uint16_t GameStateTracker::memoryCopy[1024 * 16];
size_t GameStateTracker::maxAddress = 0;
size_t GameStateTracker::oldMaxAddress = 0;
uint16_t GameStateTracker::oldMaxValue = 0;
uint16_t GameStateTracker::invocationCounter = 0;
std::chrono::time_point<std::chrono::steady_clock> GameStateTracker::oldTime;
std::chrono::time_point<std::chrono::steady_clock>
    GameStateTracker::highscoreEnterTime;
std::chrono::time_point<std::chrono::steady_clock>
    GameStateTracker::currentTime;
size_t GameStateTracker::playToIntroCount = 0;
size_t GameStateTracker::playToHighCount = 0;

void GameStateTracker::identifyGameState(uint16_t prgCounter)
{
    bool doGameStateStep = false;
    // Keep track of the instruction executed more often.
    if (++memoryCopy[prgCounter] > oldMaxValue) maxAddress = prgCounter;

    // Check elapsed time each amount of executed instructions.
    if ((++invocationCounter & 0xFFF) == 0)
    {
        currentTime = std::chrono::steady_clock::now();
        auto elapsed = (currentTime - oldTime).count();
        // Each amount of seconds, compute the game state.
        if (elapsed > .5)  // seconds elapsed
        {
            oldTime = currentTime;

            oldMaxAddress = maxAddress;
            oldMaxValue = memoryCopy[prgCounter];

#if defined(DEBUG)
            // std::cout << " max:" << std::hex << maxAddress << '\n';
#endif

            memset(memoryCopy, 0, sizeof(memoryCopy));
            maxAddress = 0;
            doGameStateStep = true;
        }

        if (doGameStateStep)
        {
            switch (gameState)
            {
                case NONE:
                {
                    // Transition to INTRO immediately
                    gameState = INTRO;
                    break;
                }

                case INTRO:
                {
                    if ((oldMaxAddress == 0x416) || (oldMaxAddress == 0x1bf) ||
                        (oldMaxAddress == 0x5b4) || (oldMaxAddress == 0x1337))
                    {
                        // Transition to PLAY.
                        playToIntroCount = 0;
                        playToHighCount = 0;
                        gameState = PLAY;
#if defined(DEBUG)
                        std::cout << " INTRO->PLAY\n";
#endif
                        break;
                    }
                    break;
                }  // case

                case PLAY:
                {
                    if ((fabs((oldMaxAddress - 0x30aa) < 16) ||
                         (fabs(oldMaxAddress - 0x32ed) < 32) ||
                         (fabs(oldMaxAddress - 0x30c5) < 16)) &&
                        (++playToHighCount > 3))
                    {
                        // Transition to HIGHSCORE
                        highscoreEnterTime = std::chrono::steady_clock::now();
                        gameState = HIGHSCORE;
#if defined(DEBUG)
                        std::cout << " PLAY->HIGHSCORE\n";
#endif
                        break;
                    }
                    if ((std::fabs(oldMaxAddress - 0xa7) < 16) ||
                        (std::fabs(oldMaxAddress - 0x3db) < 16))
                    {
                        if (playToIntroCount++ > 5)  // sec elapsed.
                        {
                            // Transition to INTRO.
                            gameState = INTRO;
#if defined(DEBUG)
                            std::cout << " HIGHSCORE->INTO\n";
#endif
                            break;
                        }
                    }
                    break;
                }

                case HIGHSCORE:
                {
                    if ((std::chrono::steady_clock::now() - highscoreEnterTime)
                            .count() > 5)
                    {
                        if (std::fabs(oldMaxAddress - 0xa7) < 32)
                        {
                            // Transition to INTRO.
                            gameState = INTRO;
#if defined(DEBUG)
                            std::cout << " HIGHSCORE->INTRO\n";
#endif
                        }
                    }
                    break;
                }
            }

#if defined(DEBUG)
                // std::cout << " state:" << gameState << '\n';
#endif
        }  // if
    }      // if
}
