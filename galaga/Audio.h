#pragma once

#include "emulator/Timer.h"

namespace Galaga
{
// forward declarations:
class GalagaMachine;

class Audio
{
   public:
    Audio(GalagaMachine& galagaMachine);
    ~Audio();

    void update(int64_t currentTime);

    std::vector<int> GetAudio() { return std::move(_buffer); }

   private:
    GalagaMachine& _galagaMachine;
    Emulator::CTimer* _timer;
    std::vector<int> _buffer;
};

}  // namespace Galaga