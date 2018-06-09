#pragma once
#include <stdint.h>
#include <string.h>
#include <vector>


namespace Galaga
{
class Frame
{
    Frame& operator=(const Frame&) = delete;
    Frame(const Frame&) = delete;

   public:
    Frame(int width, int height) : Width(width), Height(height), Pixels(nullptr)
    {
        Pixels = new uint8_t[width * height * 4];
    }
    ~Frame() { delete[] Pixels; }

    void SetPixel(int x, int y, int color)
    {
        uint8_t r = (color & 0xff0000) >> 16;
        uint8_t g = (color & 0xff00) >> 8;
        uint8_t b = (color & 0xff);
        uint32_t pixelIdx = ((Width * y) + x) * 4;
        Pixels[pixelIdx++] = r;
        Pixels[pixelIdx++] = g;
        Pixels[pixelIdx++] = b;
        Pixels[pixelIdx++] = 0xff;
    }

    void Clear() { memset(Pixels, 0, Width * Height * 4); }

   public:
    int Width;
    int Height;
    uint8_t* Pixels;
};

class AudioFrame
{
    AudioFrame& operator=(const AudioFrame&) = delete;
    AudioFrame(const AudioFrame&) = delete;

   public:
    AudioFrame() : Size(0), Samples(nullptr) {}

    ~AudioFrame()
    {
        if (Samples)
        {
            delete[] Samples;
        }
    }

    void Set(const std::vector<int>& samples)
    {
        delete[] Samples;
        Size = samples.size();
        Samples = new int[samples.size()];
        memcpy(Samples, samples.data(), samples.size() * sizeof(int));
    }

   public:
    size_t Size;
    int* Samples;
};

}  // namespace Galaga
