#pragma once

#include "emulator/Cpu.h"
#include "emulator/emulator.h"
#include "mb88xx/mb88_processor.h"


namespace Galaga
{
class GalagaMachine;

class Namco51 : public Emulator::Cpu
{
    typedef Emulator::Cpu Inherited;

    enum Mode
    {
        Switch = 0,
        Credits = 1,
        Play = 2
    };

    class IO : public Emulator::IOHandler
    {
       public:
        IO(const Namco51& mcu, GalagaMachine& galagaMachine);

        // IOHandler
        void Write(int port, uint8_t data);
        uint8_t Read(int port);

       private:
        // const Namco51& mcu;
        Emulator::IInputPort* readHandlers[4];
        std::function<void(uint8_t)> writeHandlers[2];
    };

   public:
    Namco51(GalagaMachine& galagaMachine);
    ~Namco51();

    // IMemoryHandler
    virtual bool WriteMemory(uint16_t address, uint8_t value);
    virtual bool ReadMemory(uint16_t address, uint8_t& value);

   protected:
    uint8_t ReadPort(int port);
    void WritePort(int port, uint8_t data);

   private:
    static const uint32_t RomLength = 0x0400;
    static const uint32_t RamLength = 0x40;

    // GalagaMachine& galagaMachine;
    IO ioHandler;
    uint8_t rom[RomLength];
    uint8_t ram[RamLength];

    // state
    Mode mode;
    int coincredMode;
    int credits;
    uint8_t coinsPerCred[2];
    uint8_t credsPerCoin[2];
    int coins[2];
    bool fRemapJoystick;
    uint8_t lastCoins;
    uint8_t lastButtons;

    static const uint8_t s_joyMap[16];
};

}  // namespace Galaga
