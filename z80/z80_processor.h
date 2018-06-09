#pragma once

#include <memory>
#include "../emulator/emulator.h"
#include "emulator/DeviceInput.h"


namespace Z80
{
class Z80ProcessorImpl;

class Processor : public Emulator::IProcessor
{
   public:
    Processor(double clockFrequency, uint8_t ram[],
              Emulator::IOHandler& ioHandler, const std::string tag,
              Emulator::IMemoryHandler* memoryHandler, Emulator::Cpu& cpu);
    ~Processor() override;

    // IProcessor
    void Reset() override;
    int64_t get_ExecutionTimeInCurrentTimeslice() const override;
    int64_t Execute(int64_t us) override;
    void AbortTimeslice() override;
    Emulator::IDeviceInputLine& get_IrqInputLine() override;
    Emulator::IDeviceInputLine& get_ResetInputLine() override;
    Emulator::IDeviceInputLine& get_NmiInputLine() override;
    std::vector<Emulator::IDeviceInputLine*> get_InputLines() override;
    void SetProgramCounterListener(ProgramCounterListener&) override;

   private:
    std::unique_ptr<Z80ProcessorImpl> _impl;
};

}  // namespace Z80