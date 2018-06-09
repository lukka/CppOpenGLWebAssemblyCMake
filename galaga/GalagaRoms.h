#pragma once

#include <map>
#include <string>
#include "emulator/emulator.h"


namespace Galaga
{
class GalagaRoms
{
   public:
    GalagaRoms();
    virtual const ByteArray& ReadRom(const std::string& rom);

   private:
    static uint8_t* rom_51xx;
    void LoadFile(std::string const& fileName, std::vector<uint8_t>& out);

    typedef std::map<std::string, ByteArray> Roms;
    Roms roms;
};
}  // namespace Galaga