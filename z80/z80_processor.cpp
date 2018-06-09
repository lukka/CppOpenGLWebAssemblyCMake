#include "z80_processor.h"

#include "z80_processor_impl.h"

namespace Z80
{
Processor::Processor(double clockFrequency, uint8_t ram[],
                     Emulator::IOHandler& ioHandler, const std::string tag,
                     Emulator::IMemoryHandler* memoryHandler,
                     Emulator::Cpu& cpu)
    : _impl(new Z80ProcessorImpl(clockFrequency, ram, ioHandler, tag,
                                 memoryHandler, cpu))
{
}

Processor::~Processor() {}

void Processor::Reset() { _impl->Reset(); }
int64_t Processor::get_ExecutionTimeInCurrentTimeslice() const
{
    return _impl->get_ExecutionTimeInCurrentTimeslice();
}
int64_t Processor::Execute(int64_t us) { return _impl->Execute(us); }
void Processor::AbortTimeslice() { _impl->AbortTimeslice(); }
Emulator::IDeviceInputLine& Processor::get_IrqInputLine()
{
    return _impl->get_IrqInputLine();
}
Emulator::IDeviceInputLine& Processor::get_ResetInputLine()
{
    return _impl->get_ResetInputLine();
}
Emulator::IDeviceInputLine& Processor::get_NmiInputLine()
{
    return _impl->get_NmiInputLine();
}
std::vector<Emulator::IDeviceInputLine*> Processor::get_InputLines()
{
    return _impl->get_InputLines();
}
void Processor::SetProgramCounterListener(ProgramCounterListener& pcListener)
{
    _impl->SetProgramCounterListener(pcListener);
}
}  // namespace Z80
