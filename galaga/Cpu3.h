#pragma once

#include "emulator/Cpu.h"
#include "emulator/emulator.h"


namespace Galaga
{
class GalagaMachine;

// ROM_REGION( 0x10000, "sub", 0 )     // 64k for the second CPU
// ROM_LOAD( "3600e.bin",    0x0000, 0x1000, CRC(bc556e76)
// SHA1(0d3d68243c4571d985b4d8f7e0ea9f6fcffa2116) )
class Cpu3 : public Emulator::Cpu
{
    typedef Emulator::Cpu Inherited;

   public:
    Cpu3(GalagaMachine& galaga, uint8_t* ram, uint32_t ramLength,
         Emulator::IOHandler& ioHandler);
    virtual ~Cpu3();

    // IMemoryHandler
    virtual bool WriteMemory(uint16_t address, uint8_t value);
    virtual bool ReadMemory(uint16_t address, uint8_t& value);

    void NmiLinePulse();

   private:
    GalagaMachine& galagaMachine;
    static const uint32_t romLength = 4 * 1024;
    uint8_t rom[romLength];
    // uint32_t amLength;
    uint8_t* ram;
};

}  // namespace Galaga
