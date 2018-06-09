#pragma once

#include <memory>
#include "Tile.h"
#include "emulator/Timer.h"
#include "emulator/emulator.h"


namespace Galaga
{
class GalagaMachine;
class Starfield;

class ScreenDevice
{
    static const int ScreenTotalHeight = 264;
    static const int64_t FramePeriod = (long)(1e6 / 60);  // 60Hz (in us)
    static const int64_t ScanTime = FramePeriod / ScreenTotalHeight;

    // visible area
   public:
    static const int Width = 288;
    static const int Columns = Width / 8;
    static const int Height = 224;
    static const int Rows = Height / 8;

   private:
    static const int VisibleAreaFirstLine = (ScreenTotalHeight - Height) / 2;
    static const int VisibleAreaLastLine = VisibleAreaFirstLine + Height - 1;
    static const int VBlank0Line = VisibleAreaLastLine + 1;
    static const int TotSprites = 64;
    static const int TotTiles = 8192;

    static const uint16_t VideoRamBase0 = 0x8000;
    static const uint16_t VideoRamLength0 = 0x800;
    static const uint16_t VideoRamBase1 = 0x8800;
    static const uint16_t VideoRamLength1 = 0x400;
    static const uint16_t VideoRamBase2 = 0x9000;
    static const uint16_t VideoRamLength2 = 0x400;
    static const uint16_t VideoRamBase3 = 0x9800;
    static const uint16_t VideoRamLength3 = 0x400;

    struct TileData
    {
        uint8_t code;   // 7 bits
        uint8_t color;  // 6 bits

        TileData() : code(255), color(0) {}

        int Index() const { return code << 6 | color; }
        operator bool() const { return code != 255; }
        bool operator==(const TileData& rhs) const
        {
            return code == rhs.code && color == rhs.color;
        }
    };

    struct SpriteData
    {
        int code;   // 7 bits
        int color;  // 6 bits
        bool flipx;
        bool flipy;
        uint8_t sizex;
        uint8_t sizey;

        bool operator==(const SpriteData& rhs) const
        {
            return (rhs.code == code && rhs.color == color &&
                    rhs.flipx == flipx && rhs.flipy == flipy &&
                    rhs.sizex == sizex && rhs.sizey == sizey);
        }
        bool operator!=(const SpriteData& rhs) const
        {
            return !operator==(rhs);
        }
        bool operator<(const SpriteData& rhs) const
        {
            uint16_t l = (((((code | (color << 8)) | (flipx ? 0x10000 : 0)) |
                            (flipy ? 0x20000 : 0)) |
                           ((sizex / 0x10) << 0x12)) |
                          ((sizey / 0x10) << 20));
            uint16_t r =
                (((((rhs.code | (rhs.color << 8)) | (rhs.flipx ? 0x10000 : 0)) |
                   (rhs.flipy ? 0x20000 : 0)) |
                  ((rhs.sizex / 0x10) << 0x12)) |
                 ((rhs.sizey / 0x10) << 20));

            return l < r;
        }
    };

   public:
    ScreenDevice(GalagaMachine* galaga, uint8_t* ram);
    virtual ~ScreenDevice();

    void DrawSprites(Frame& frame);
    void DrawStars(Frame& frame);
    void DrawTiles(Frame& frame);

    int64_t TimeUntilScanline(int scanline) const;

    bool WriteMemory(uint16_t address, uint8_t value);

    Starfield* getStarfield() const { return starfield; }

   protected:
    void Reset();
    bool InitRoms();
    int GenerateTile(int code, int colorIndex);
    std::unique_ptr<Sprite> GenerateSprite(SpriteData spriteData) const;
    void GetTileInfo(int tileIndex, int& code, int& color);
    static int MapTile(int col, int row, int totColumns, int totRows);
    void InitMappings();
    void UpdateMappings();
    void MarkTileDirty(int memIndex);
    void OnVblankBegin(int64_t param);
    int64_t get_TimeUntilVblankStart() const;

    static bool readbit(const ByteArray& src, int bitnum)
    {
        return 0 != (src[bitnum / 8] & (0x80 >> (bitnum % 8)));
    }

   private:
    GalagaMachine* galaga;
    uint8_t* ram;
    ByteArray romTxt;
    ByteArray romSprites;
    int* romPalette;
    ByteArray romColorTable;

    int tileMap[Rows][Columns];
    bool* dirtyMap;
    int maxMemoryIndex;
    int maxLogicalIndex;
    int* memoryToLogical;
    int* logicalToMemory;

    Tile tileCache[TotTiles];
    TileData tileData[TotTiles];

    std::unique_ptr<Sprite> sprites[TotSprites];
    SpriteData spritesData[TotSprites];

    Starfield* starfield;
    bool flipScreen;  // ignored

    Emulator::CTimer* vblankBeginTimer;
    long vblankStartTime;
};

}  // namespace Galaga
