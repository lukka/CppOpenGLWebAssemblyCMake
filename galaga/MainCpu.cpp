#include "MainCpu.h"
#include <algorithm>
#include "GalagaMachine.h"
#include "Namco06.h"
#include "ScreenDevice.h"
#include "z80/z80_processor.h"


namespace Galaga
{
MainCpu::MainCpu(GalagaMachine& galaga, uint8_t* ram, uint32_t ramLength,
                 Emulator::IOHandler& ioHandler)
    :
#pragma warning( \
    disable : 4355)  // 'this' : used in base member initializer list
      Inherited(galaga.scheduler, 0,
                new Z80::Processor(galaga.ClockFrequency, ram, ioHandler,
                                   "main", this, *this)),
#pragma warning(default : 4355)
      galagaMachine(galaga),
      // ramLength(ramLength),
      ram(ram)
{
    ByteArray rom3200a = galaga.ReadRom("3200a.bin");
    std::copy(begin(rom3200a), end(rom3200a), rom);

    ByteArray rom3300b = galaga.ReadRom("3300b.bin");
    for (int i = 0; i < 4096; i++)
    {
        rom[4 * 1024 + i] = rom3300b[i];
    }

    ByteArray rom3400c = galaga.ReadRom("3400c.bin");
    for (int i = 0; i < 4096; i++)
    {
        rom[8 * 1024 + i] = rom3400c[i];
    }

    ByteArray rom3500d = galaga.ReadRom("3500d.bin");
    for (int i = 0; i < 4096; i++)
    {
        rom[12 * 1024 + i] = rom3500d[i];
    }

    galagaMachine.scheduler.RegisterDevice(this);
}

MainCpu::~MainCpu()
{
    delete processor;
    processor = nullptr;
}

bool MainCpu::WriteMemory(uint16_t address, uint8_t value)
{
    if (address < 0x4000)
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

bool MainCpu::ReadMemory(uint16_t address, uint8_t& value)
{
    if (address < 0x4000)
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

void MainCpu::OnVblankStart()
{
    if (!IsSuspended(Emulator::SuspendReason::SUSPEND_REASON_HALT |
                     Emulator::SuspendReason::SUSPEND_REASON_RESET |
                     Emulator::SuspendReason::SUSPEND_REASON_DISABLE))
    {
        get_Processor().get_IrqInputLine().SetState(
            Emulator::LineState::Assert);
    }
}

}  // namespace Galaga
