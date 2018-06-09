#pragma once

#include <exception>

#include "emulator/emulator.h"

namespace Galaga
{
class GalagaMachine;

class GalagaIOHandler : public Emulator::IOHandler
{
   public:
    GalagaIOHandler(const GalagaMachine* galaga) {}
    virtual ~GalagaIOHandler() {}

    // Emulator::IOHandler
    virtual void Write(int port, uint8_t value)
    {
        // throw new std::exception("should never be called");
        return;
    }
    virtual uint8_t Read(int port)
    {
        // throw new std::exception("should never be called");
        return 0;
    }
};

}  // namespace Galaga
