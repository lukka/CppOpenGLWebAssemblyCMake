#pragma once

#include "Namco51.h"
#include "Namco54.h"
#include "emulator/Timer.h"
#include "emulator/emulator.h"


namespace Galaga
{
class MainCpu;
class GalagaMachine;

class Namco06
{
   public:
    Namco06(GalagaMachine& galagaMachine);
    virtual ~Namco06();

    void WriteMemory(uint16_t offset, uint8_t data);
    uint8_t ReadMemory(uint16_t offset);

   private:
    static const int IndexNamco51Device = 0;
    static const int IndexNamco54Device = 3;
    uint8_t control;
    Namco51 namco51;
    Namco54 namco54;
    MainCpu& mainCpu;
    Emulator::CTimer* timer;
};

}  // namespace Galaga
