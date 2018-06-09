#pragma once

#include "emulator/emulator.h"

namespace Galaga
{
class Frame;

class Starfield
{
    struct Star
    {
        uint16_t x;
        uint16_t y;
        uint8_t col;
        uint8_t set;

        Star(uint16_t x, uint16_t y, uint8_t col, uint8_t set)
            : x(x), y(y), col(col), set(set)
        {
        }
    };

   public:
    Starfield();
    ~Starfield() {}

    void DrawStars(Frame& frame);
    void SetControl(int offset, uint8_t data) { _control[offset] = data; }

   private:
    void Move();

   private:
    static const Star starSeedTab[];
    static const size_t PaletteSize = 0x40;
    uint32_t palette[PaletteSize];
    uint8_t _control[6];
    int _starsScrollX;
};

}  // namespace Galaga
