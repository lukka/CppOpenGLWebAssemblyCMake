#include "Cpu3.h"
#include <algorithm>
#include "GalagaMachine.h"
#include "Namco06.h"
#include "ScreenDevice.h"
#include "z80/z80_processor.h"


namespace Galaga
{
Cpu3::Cpu3(GalagaMachine& galaga, uint8_t* ram, uint32_t ramLength,
           Emulator::IOHandler& ioHandler)
    :
#pragma warning( \
    disable : 4355)  // 'this' : used in base member initializer list
      Inherited(galaga.scheduler, 1,
                new Z80::Processor(galaga.ClockFrequency, ram, ioHandler,
                                   "cpu3", this, *this)),
#pragma warning(default : 4355)
      galagaMachine(galaga),
      // ramLength(ramLength),
      ram(ram)
{
    ByteArray rom3700g = galaga.ReadRom("3700g.bin");
    std::copy(begin(rom3700g), end(rom3700g), rom);

    galagaMachine.scheduler.RegisterDevice(this);
}

Cpu3::~Cpu3()
{
    delete processor;
    processor = nullptr;
}

bool Cpu3::WriteMemory(uint16_t address, uint8_t value)
{
    if (address < 0x1000)
    {
        // rom - do nothing
        return false;
    }
    else if (address >= 0x6800 && address < 0x6820)
    {
        galagaMachine.getNamcoSound().WriteMemory((uint16_t)(address - 0x6800),
                                                  value);
        return true;
    }
    else if (address >= 0x6820 && address < 0x6828)
    {
        galagaMachine.WriteLatch(address - 0x6820, value);
    }
    else if (address >= 0x7000 && address < 0x7101)
    {
        galagaMachine.namco06xx->WriteMemory((uint16_t)(address - 0x7000),
                                             value);
        return true;
    }
    else if (!galagaMachine.screenDevice->WriteMemory(address, value))
    {
        ram[address] = value;
    }

    return true;
}

bool Cpu3::ReadMemory(uint16_t address, uint8_t& value)
{
    if (address < 0x1000)
    {
        value = rom[address];
        return true;
    }
    else if (address >= 0x6800 && address < 0x6808)
    {
        value = galagaMachine.ReadDsw(address - 0x6800);
        return true;
    }
    else if (address >= 0x7000 && address < 0x7101)
    {
        value =
            galagaMachine.namco06xx->ReadMemory((uint16_t)(address - 0x7000));
        return true;
    }
    else
    {
        value = ram[address];
        return true;
    }
}

void Cpu3::NmiLinePulse()
{
    if (get_IsInterruptEnabled())
    {
        processor->get_NmiInputLine().SetState(
            Emulator::LineState::Pulse,
            Emulator::_DeviceInput::USE_STORED_VECTOR);
    }
}

}  // namespace Galaga
