#include "ScreenDevice.h"
#include <algorithm>
#include "GalagaMachine.h"
#include "Starfield.h"
#include "emulator/Timer.h"


namespace Galaga
{
ScreenDevice::ScreenDevice(GalagaMachine* galaga, uint8_t* ram)
    : galaga(galaga),
      ram(ram),
      romTxt(),
      romSprites(),
      romPalette(NULL),
      romColorTable(),
      dirtyMap(NULL),
      maxMemoryIndex(0),
      maxLogicalIndex(0),
      memoryToLogical(NULL),
      logicalToMemory(NULL),
      starfield(new Starfield),
      flipScreen(false),
      vblankBeginTimer(nullptr)
{
    // memset(tileCache, 0, sizeof(Tile) * TotTiles);

    for (int i = 0; i < TotSprites; i++)
    {
        sprites[i] = nullptr;
    }

    InitMappings();
    InitRoms();

    vblankBeginTimer = new Emulator::CTimer(
        "OnVblankBegin", galaga->scheduler,
        [this](int64_t param, int64_t currentTime) { OnVblankBegin(param); }),
    Reset();
}

ScreenDevice::~ScreenDevice()
{
    delete vblankBeginTimer;
    delete starfield;
}

void ScreenDevice::Reset()
{
    vblankBeginTimer->Start(get_TimeUntilVblankStart(), false);
}

bool ScreenDevice::InitRoms()
{
    romTxt = galaga->ReadRom("2600j.bin");

    ByteArray romSprites0 = galaga->ReadRom("2800l.bin");
    ByteArray romSprites1 = galaga->ReadRom("2700k.bin");
    romSprites.resize(romSprites0.size() + romSprites1.size());
    std::copy(begin(romSprites0), end(romSprites0), begin(romSprites));
    for (size_t i = 0; i < romSprites1.size(); i++)
    {
        romSprites[romSprites0.size() + i] = romSprites1[i];
    }

    ByteArray romColorTable0 = galaga->ReadRom("prom-4.2n");
    ByteArray romColorTable1 = galaga->ReadRom("prom-3.1c");
    romColorTable.resize(romColorTable0.size() + romColorTable1.size());
    std::copy(begin(romColorTable0), end(romColorTable0), begin(romColorTable));
    for (size_t i = 0; i < romColorTable1.size(); i++)
    {
        romColorTable[romColorTable0.size() + i] = romColorTable1[i];
    }

    ByteArray rom = galaga->ReadRom("prom-5.5n");
    romPalette = new int[32];
    for (int i = 0; i < 32; i++)
    {
        int bit0, bit1, bit2;
        uint8_t r, g, b;

        bit0 = (rom[i] >> 0) & 0x01;
        bit1 = (rom[i] >> 1) & 0x01;
        bit2 = (rom[i] >> 2) & 0x01;
        r = (uint8_t)(0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2);
        bit0 = (rom[i] >> 3) & 0x01;
        bit1 = (rom[i] >> 4) & 0x01;
        bit2 = (rom[i] >> 5) & 0x01;
        g = (uint8_t)(0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2);
        bit0 = 0;
        bit1 = (rom[i] >> 6) & 0x01;
        bit2 = (rom[i] >> 7) & 0x01;
        b = (uint8_t)(0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2);

        romPalette[i] = (255 << 24) | (r << 16) | (g << 8) | b;
    }

    // set transparent color
    romPalette[15] &= 0x00ffffff;
    romPalette[31] &= 0x00ffffff;

    return true;
}

int ScreenDevice::GenerateTile(int code, int colorIndex)
{
    SpriteData data;
    data.code = code;
    data.color = colorIndex;
    data.flipx = data.flipy = false;
    data.sizex = data.sizey = 8;

    int index = code << 6 | colorIndex;
    assert(index < TotTiles);
    if (tileData[index])
    {
        return index;
    }

    int yOffset[] = {0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38};
    int xOffset[] = {0x40, 0x41, 0x42, 0x43, 0x00, 0x01, 0x02, 0x03};
    int pOffset[2] = {0, 4};
    const int planes = 2;
    int charincrement = 128;
    int dp[64];
    memset(dp, 0, 64 * sizeof(int));
    for (int plane = 0; plane < planes; plane++)
    {
        uint8_t planebit = (uint8_t)(1 << (planes - 1 - plane));
        int planeoffs = code * charincrement + pOffset[plane];
        int line_modulo = 8;

        for (int y = 0; y < 8; y++)
        {
            int yoffs = planeoffs + yOffset[y];

            for (int x = 0; x < 8; x += 2)
            {
                // int iuint8_t = xOffset[x];

                if (readbit(romTxt, yoffs + xOffset[x]))
                {
                    dp[y * line_modulo + x] |= planebit;
                }

                if (readbit(romTxt, yoffs + xOffset[x + 1]))
                {
                    dp[y * line_modulo + x + 1] |= planebit;
                }
            }
        }
    }

    Tile* tile = &tileCache[index];
    int i = 0;
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++, i++)
        {
            int iCt = dp[y * 8 + x] + 4 * colorIndex;
            tile->pixels[i] = romPalette[16 + romColorTable[iCt]];
        }
    }

    return index;
}

std::unique_ptr<Sprite> ScreenDevice::GenerateSprite(
    SpriteData spriteData) const
{
    int offs[2][2] = {{0, 1}, {2, 3}};
    int yOffset[] = {0x00, 0x01, 0x02, 0x03, 0x40, 0x41, 0x42, 0x43,
                     0x80, 0x81, 0x82, 0x83, 0xc0, 0xc1, 0xc2, 0xc3};
    int xOffset[] = {0x00,  0x08,  0x10,  0x18,  0x20,  0x28,  0x30,  0x38,
                     0x100, 0x108, 0x110, 0x118, 0x120, 0x128, 0x130, 0x138};
    int pOffset[2] = {0, 4};
    const int planes = 2;
    const int charincrement = 512;

    auto newSprite = new Sprite(spriteData.sizex, spriteData.sizey);

    for (int i = 0; i < (spriteData.sizey / 16); i++)
    {
        for (int j = 0; j < (spriteData.sizex / 16); j++)
        {
            int offset = offs[i][j];
            int code = spriteData.code + offset;
            int dp[256];
            memset(dp, 0, 256 * sizeof(int));
            for (int plane = 0; plane < planes; plane++)
            {
                int planebit = 1 << (planes - 1 - plane);
                int planeoffs = code * charincrement + pOffset[plane];
                int line_modulo = 16;

                for (int y = 0; y < 16; y++)
                {
                    int yoffs = planeoffs + yOffset[y];

                    for (int x = 0; x < 16; x += 2)
                    {
                        // int iuint8_t = xOffset[x];

                        if (readbit(romSprites, yoffs + xOffset[x]))
                        {
                            dp[y * line_modulo + x] |= planebit;
                        }

                        if (readbit(romSprites, yoffs + xOffset[x + 1]))
                        {
                            dp[y * line_modulo + x + 1] |= planebit;
                        }
                    }
                }
            }

            int startx =
                spriteData.flipx ? (((spriteData.sizex / 16) - 1) - j) : j;
            int starty =
                spriteData.flipy ? (((spriteData.sizey / 16) - 1) - i) : i;
            int index = starty * spriteData.sizex * 16 + startx * 16;
            int incx = 1;
            int incy = spriteData.sizex - 16;
            if (spriteData.flipy)
            {
                index += spriteData.sizex * 15;
                incy = -spriteData.sizex - 16;
            }
            if (spriteData.flipx)
            {
                index += 15;
                incx = -1;
                incy = spriteData.flipy ? (-spriteData.sizex + 16)
                                        : (spriteData.sizex + 16);
            }

            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    int iCt = dp[x * 16 + y] +
                              4 * spriteData.color;  // code review! (rotated?)
                    int iCt2 = romColorTable[256 + iCt];
                    newSprite->pixels[index] = romPalette[iCt2];
                    index += incx;
                }
                index += incy;
            }
        }
    }

    return std::unique_ptr<Sprite>(newSprite);
}

void ScreenDevice::DrawSprites(Frame& frame)
{
    for (int index = 0; index < TotSprites; index++)
    {
        SpriteData spriteData;
        spriteData.code = ram[VideoRamBase1 + 0x380 + 2 * index] & 0x7f;
        spriteData.color = ram[VideoRamBase1 + 0x380 + 2 * index + 1] & 0x3f;

        int left = ram[VideoRamBase2 + 0x380 + 2 * index + 1] - 40 +
                   0x100 * (ram[VideoRamBase3 + 0x380 + 2 * index + 1] & 0x03);
        int top = 256 - ram[VideoRamBase2 + 0x380 + 2 * index] +
                  1;  // sprites are buffered and delayed by one scanline
        spriteData.flipx = 0 != (ram[VideoRamBase3 + 0x380 + 2 * index] & 0x01);
        spriteData.flipy =
            0 != ((ram[VideoRamBase3 + 0x380 + 2 * index] & 0x02) >> 1);
        spriteData.sizex = (uint8_t)(
            16 * (1 + ((ram[VideoRamBase3 + 0x380 + 2 * index] & 0x04) >> 2)));
        spriteData.sizey = (uint8_t)(
            16 * (1 + ((ram[VideoRamBase3 + 0x380 + 2 * index] & 0x08) >> 3)));

        top -= spriteData.sizey - 16;
        top = (top & 0xff) - 32;  // fix wraparound

        if (nullptr == sprites[index] || spritesData[index] != spriteData)
        {
            spritesData[index] = spriteData;
            sprites[index] = GenerateSprite(spriteData);
        }

        sprites[index]->Draw(frame, left, top);
    }
}

void ScreenDevice::DrawStars(Frame& frame) { starfield->DrawStars(frame); }

void ScreenDevice::DrawTiles(Frame& frame)
{
    for (int y = 0; y < Rows; y++)
    {
        for (int x = 0; x < Columns; x++)
        {
            int logindex = y * Columns + x;
            if (dirtyMap[logindex])
            {
                int code = 0;
                int color = 0;
                GetTileInfo(logindex, code, color);
                tileMap[y][x] = GenerateTile(code, color);
                dirtyMap[logindex] = false;
            }
            tileCache[tileMap[y][x]].Draw(frame, x * 8, y * 8);
        }
    }
}

void ScreenDevice::GetTileInfo(int tileIndex, int& code, int& color)
{
    int memIndex = logicalToMemory[tileIndex];
    code = ram[VideoRamBase0 + memIndex] & 0x7f;
    color = ram[VideoRamBase0 + memIndex + 0x400] & 0x3f;
}

// static
int ScreenDevice::MapTile(int col, int row, int totColumns, int totRows)
{
    row += 2;
    col -= 2;
    if (0 != (col & 0x20))
    {
        return (row + ((col & 0x1f) << 5));
    }
    else
    {
        return (col + (row << 5));
    }
}

void ScreenDevice::InitMappings()
{
    maxLogicalIndex = Rows * Columns;

    // compute the maximum memory index
    maxMemoryIndex = 0;
    for (int row = 0; row < Rows; row++)
    {
        for (int col = 0; col < Columns; col++)
        {
            int memIndex = MapTile(col, row, Columns, Rows);
            maxMemoryIndex =
                maxMemoryIndex > memIndex ? maxMemoryIndex : memIndex;
        }
    }
    maxMemoryIndex++;

    assert(nullptr == memoryToLogical);
    memoryToLogical = new int[maxMemoryIndex];

    assert(nullptr == logicalToMemory);
    logicalToMemory = new int[maxLogicalIndex];

    assert(nullptr == dirtyMap);
    dirtyMap = new bool[maxLogicalIndex];

    UpdateMappings();
}

void ScreenDevice::UpdateMappings()
{
    // initialize all the mappings to invalid values
    for (int memindex = 0; memindex < maxMemoryIndex; memindex++)
    {
        memoryToLogical[memindex] = -1;
    }

    // now iterate over all logical indexes and populate the memory index
    for (int logindex = 0; logindex < maxLogicalIndex; logindex++)
    {
        int logicalCol = logindex % Columns;
        int logicalRow = logindex / Columns;
        int memindex = MapTile(logicalCol, logicalRow, Columns, Rows);

        int flippedLogIndex = logicalRow * Columns + logicalCol;

        // fill in entries in both arrays
        memoryToLogical[memindex] = flippedLogIndex;
        logicalToMemory[flippedLogIndex] = memindex;
    }

    // mark the whole tilemap dirty
    for (int i = 0; i < maxLogicalIndex; i++)
    {
        dirtyMap[i] = true;
    }
}

void ScreenDevice::MarkTileDirty(int memIndex)
{
    if (memIndex < maxMemoryIndex)
    {
        int logindex = memoryToLogical[memIndex];
        if (logindex != -1)
        {
            dirtyMap[logindex] = true;
        }
    }
}

bool ScreenDevice::WriteMemory(uint16_t address, uint8_t value)
{
    if (address >= VideoRamBase0 && address < VideoRamBase0 + VideoRamLength0)
    {
        uint16_t offset = (uint16_t)(address - VideoRamBase0);
        ram[address] = value;

        if (offset < Width * Height)
        {
            MarkTileDirty(offset & 0x3ff);
        }

        return true;
    }
    else if ((address >= 0xa000) && (address < 0xa006))
    {
        starfield->SetControl(address - 0xa000, value);
        ram[address] = value;
        return true;
    }
    else if (address == 0xa007)
    {
        flipScreen = (value != 0);
        return true;
    }

    return false;
}

void ScreenDevice::OnVblankBegin(int64_t param)
{
    vblankStartTime = galaga->scheduler.get_CurrentTime();
    vblankBeginTimer->Start(get_TimeUntilVblankStart(), false);

    galaga->OnVblankStartCallback();
}

int64_t ScreenDevice::TimeUntilScanline(int scanline) const
{
    int64_t vpos =
        (scanline + ScreenTotalHeight - VBlank0Line) % ScreenTotalHeight;
    int64_t targetDelta = vpos * ScanTime;  // us
    int64_t currentDelta =
        galaga->scheduler.get_CurrentTime() - vblankStartTime;  // us
    while (targetDelta <= currentDelta)
    {
        targetDelta += FramePeriod;
    }
    return (targetDelta - currentDelta);  // us
}

int64_t ScreenDevice::get_TimeUntilVblankStart() const
{
    return TimeUntilScanline(VBlank0Line);
}

}  // namespace Galaga