#include "Tile.h"

namespace Galaga
{
void Tile::Draw(Frame &frame, int left, int top)
{
    int ymin = top < 0 ? -top : 0;
    int ymax =
        top + Height > frame.Height ? frame.Height - (top + Height) : Height;
    int xmin = left < 0 ? -left : 0;
    int xmax =
        left + Width > frame.Width ? frame.Width - (left + Width) : Width;

    for (int y = ymin; y < ymax; y++)
    {
        int src = Width * y;
        int dest = (top + y) * frame.Width + left;
        for (int x = xmin; x < xmax; x++)
        {
            if ((pixels[src + x] & 0xff000000) != 0)
            {
                uint8_t r = ((pixels[src + x]) & 0xff0000) >> 16;
                uint8_t g = ((pixels[src + x]) & 0xff00) >> 8;
                uint8_t b = ((pixels[src + x]) & 0xff);
                uint32_t pixelIdx = (dest + x) * 4;
                frame.Pixels[pixelIdx++] = r;
                frame.Pixels[pixelIdx++] = g;
                frame.Pixels[pixelIdx++] = b;
                frame.Pixels[pixelIdx++] = 0xff;
            }
        }
    }
}

Sprite::Sprite(int width, int height)
    : width(width), height(height), pixels(NULL)
{
    pixels = new int[width * height];
}
Sprite::~Sprite() { delete[] pixels; }

void Sprite::Draw(Frame &frame, int left, int top)
{
    int ymin = top < 0 ? -top : 0;
    int ymax =
        top + height > frame.Height ? frame.Height - (top + height) : height;
    int xmin = left < 0 ? -left : 0;
    int xmax =
        left + width > frame.Width ? frame.Width - (left + width) : width;

    for (int y = ymin; y < ymax; y++)
    {
        int src = width * y;
        int dest = (top + y) * frame.Width + left;
        for (int x = xmin; x < xmax; x++)
        {
            if ((pixels[src + x] & 0xff000000) != 0)
            {
                uint8_t r = ((pixels[src + x]) & 0xff0000) >> 16;
                uint8_t g = ((pixels[src + x]) & 0xff00) >> 8;
                uint8_t b = ((pixels[src + x]) & 0xff);
                uint32_t pixelIdx = (dest + x) * 4;
                frame.Pixels[pixelIdx++] = r;
                frame.Pixels[pixelIdx++] = g;
                frame.Pixels[pixelIdx++] = b;
                frame.Pixels[pixelIdx++] = 0xff;
            }
        }
    }
}  // Frame
}  // namespace Galaga