#include "GalagaRoms.h"
#include <cassert>
#include <iostream>
#include <vector>

namespace Galaga
{
GalagaRoms::GalagaRoms() {}

void GalagaRoms::LoadFile(std::string const& fileName,
                          std::vector<uint8_t>& out)
{
    std::string dataDir("data/");
    dataDir += fileName;
    int size = 0;
    FILE* f = fopen(dataDir.c_str(), "rb");
    assert(f != 0 && "could not open rom file!");
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    out.resize(size);
    int size2 = fread(&out[0], sizeof(uint8_t), size, f);
    assert(size == size2 && "fread failed");
    size = size2;
    std::cout << "Loaded: " << fileName << "  size " << size << "\n";
    fclose(f);
}

const ByteArray& GalagaRoms::ReadRom(const std::string& rom)
{
    Roms::const_iterator it = roms.find(rom);
    if (roms.end() == it)
    {
        std::vector<uint8_t> fileContent;
        std::cout << "Loading ROM file: " << rom << "\n";
        LoadFile(rom, fileContent);
        roms[rom] = fileContent;
        it = roms.find(rom);
    }

    return it->second;
}

GalagaRoms S_GALAGA;

}  // namespace Galaga