#pragma once

#include "emulator/Cpu.h"
#include "emulator/emulator.h"


namespace Galaga
{
class GalagaMachine;

// ROM_REGION( 0x10000, "maincpu", 0 )     // 64k for code for the first CPU
// ROM_LOAD( "3200a.bin",    0x0000, 0x1000, CRC(3ef0b053)
// SHA1(0c04a362b737998c0952a753fb3fd8c8a17e9b46) ) ROM_LOAD( "3300b.bin",
// 0x1000, 0x1000, CRC(1b280831) SHA1(f7ea12e61929717ebe43a4198a97f109845a2c62) )
// ROM_LOAD( "3400c.bin",    0x2000, 0x1000, CRC(16233d33)
// SHA1(a7eb799be5e23058754a92b15e6527bfbb47a354) ) ROM_LOAD( "3500d.bin",
// 0x3000, 0x1000, CRC(0aaf5c23) SHA1(3f4b0bb960bf002261e9c1278c88f594c6aa8ab6) )
class MainCpu : public Emulator::Cpu
{
    typedef Emulator::Cpu Inherited;

   public:
    MainCpu(GalagaMachine& galaga, uint8_t* ram, uint32_t ramLength,
            Emulator::IOHandler& ioHandler);
    virtual ~MainCpu();

    // IMemoryHandler
    virtual bool WriteMemory(uint16_t address, uint8_t value);
    virtual bool ReadMemory(uint16_t address, uint8_t& value);

    void OnVblankStart();

   private:
    GalagaMachine& galagaMachine;
    static const uint32_t romLength = 16 * 1024;
    uint8_t rom[romLength];
    // uint32_t amLength;
    uint8_t* ram;
};

}  // namespace Galaga
