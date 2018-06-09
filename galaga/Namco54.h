#pragma once

#include "DiscreteAudio.h"
#include "emulator/AudioInput.h"
#include "emulator/Cpu.h"
#include "emulator/emulator.h"
#include "mb88xx/mb88_processor.h"


namespace Galaga
{
class GalagaMachine;

class Namco54 : public Emulator::Cpu
{
    typedef Emulator::Cpu Inherited;

    class IO : public Emulator::IOHandler
    {
       public:
        IO(Namco54& mcu) : mcu(mcu) {}

        // IOHandler
        void Write(int port, uint8_t data)
        {
            switch (port)
            {
                case Mb88xx::Port::MB88_PORTO:
                {
                    uint8_t outVal = (uint8_t)(data & 0x0f);
                    if ((data & 0x10) != 0)
                    {
                        mcu.getAudioInput(1).input_write(outVal);
                    }
                    else
                    {
                        mcu.getAudioInput(0).input_write(outVal);
                    }
                }
                break;

                case Mb88xx::Port::MB88_PORTR1:
                {
                    uint8_t outVal = (uint8_t)(data & 0x0f);
                    mcu.getAudioInput(2).input_write(outVal);
                }
                break;

                default:
                    break;
            }
        }

        uint8_t Read(int port)
        {
            switch (port)
            {
                case Mb88xx::Port::MB88_PORTK:
                    return (uint8_t)(mcu.latchedCmd >> 4);

                case Mb88xx::Port::MB88_PORTR0:
                    return (uint8_t)(mcu.latchedCmd & 0x0f);

                default:
                    return 0;
            }
        }

       private:
        Namco54& mcu;
    };

   public:
    Namco54(GalagaMachine& galagaMachine);
    virtual ~Namco54();

    // IMemoryHandler
    virtual bool WriteMemory(uint16_t address, uint8_t value);
    virtual bool ReadMemory(uint16_t address, uint8_t& value);

    Emulator::AudioInput& getAudioInput(int index);

   private:
    static const uint32_t RomLength = 0x0400;
    static const uint32_t RamLength = 0x40;

    GalagaMachine& galagaMachine;
    IO ioHandler;
    uint8_t rom[RomLength];
    uint8_t ram[RamLength];
    uint8_t latchedCmd;
    Emulator::CTimer* irqClearTimer;
    Emulator::CTimer* setLatchedTimer;
};

}  // namespace Galaga
