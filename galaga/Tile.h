#pragma once
#include "Frame.h"

namespace Galaga
{
class Tile
{
   public:
    void Draw(Frame& frame, int left, int top);

    static const int Width = 8;
    static const int Height = 8;
    int pixels[Width * Height];
};

class Sprite
{
   public:
    Sprite(int width, int height);

    ~Sprite();

    void Draw(Frame& frame, int left, int top);

    int width;
    int height;
    int* pixels;
};

}  // namespace Galaga
