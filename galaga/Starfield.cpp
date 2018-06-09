#include "Starfield.h"
#include "Frame.h"

namespace Galaga
{
// static
const Starfield::Star Starfield::starSeedTab[] = {
    Star(0x85, 0x06, 0x35, 0), Star(0x8f, 0x08, 0x30, 0),
    Star(0xe5, 0x1b, 0x07, 0), Star(0x22, 0x1c, 0x31, 0),
    Star(0xe5, 0x25, 0x1d, 0), Star(0x15, 0x26, 0x29, 0),
    Star(0x80, 0x2d, 0x3b, 0), Star(0x97, 0x2e, 0x1c, 0),
    Star(0xba, 0x3b, 0x05, 0), Star(0x36, 0x3d, 0x36, 0),
    Star(0x57, 0x44, 0x09, 0), Star(0xcf, 0x44, 0x3d, 0),
    Star(0x61, 0x4e, 0x27, 0), Star(0x87, 0x64, 0x1a, 0),
    Star(0xd6, 0x64, 0x17, 0), Star(0x0b, 0x6c, 0x3c, 0),
    Star(0x06, 0x6d, 0x24, 0), Star(0x18, 0x6e, 0x3a, 0),
    Star(0xa9, 0x79, 0x23, 0), Star(0x8a, 0x7b, 0x11, 0),
    Star(0xd6, 0x80, 0x0c, 0), Star(0x67, 0x82, 0x3f, 0),
    Star(0x39, 0x83, 0x38, 0), Star(0x72, 0x83, 0x14, 0),
    Star(0xec, 0x84, 0x16, 0), Star(0x8e, 0x85, 0x10, 0),
    Star(0x20, 0x88, 0x25, 0), Star(0x95, 0x8a, 0x0f, 0),
    Star(0x0e, 0x8d, 0x00, 0), Star(0x06, 0x91, 0x2e, 0),
    Star(0x07, 0x94, 0x0d, 0), Star(0xae, 0x97, 0x0b, 0),
    Star(0x00, 0x98, 0x2d, 0), Star(0x86, 0x9b, 0x01, 0),
    Star(0x58, 0xa1, 0x34, 0), Star(0xfe, 0xa1, 0x3e, 0),
    Star(0xa2, 0xa8, 0x1f, 0), Star(0x41, 0xaa, 0x0a, 0),
    Star(0x3f, 0xac, 0x32, 0), Star(0xde, 0xac, 0x03, 0),
    Star(0xd4, 0xb9, 0x26, 0), Star(0x6d, 0xbb, 0x1b, 0),
    Star(0x62, 0xbd, 0x39, 0), Star(0xc9, 0xbe, 0x18, 0),
    Star(0x6c, 0xc1, 0x04, 0), Star(0x59, 0xc3, 0x21, 0),
    Star(0x60, 0xcc, 0x0e, 0), Star(0x91, 0xcc, 0x12, 0),
    Star(0x3f, 0xcf, 0x06, 0), Star(0xf7, 0xcf, 0x22, 0),
    Star(0x44, 0xd0, 0x33, 0), Star(0x34, 0xd2, 0x08, 0),
    Star(0xd3, 0xd9, 0x20, 0), Star(0x71, 0xdd, 0x37, 0),
    Star(0x73, 0xe1, 0x2c, 0), Star(0xb9, 0xe3, 0x2f, 0),
    Star(0xa9, 0xe4, 0x13, 0), Star(0xd3, 0xe7, 0x19, 0),
    Star(0x37, 0xed, 0x02, 0), Star(0xbd, 0xf4, 0x15, 0),
    Star(0x0f, 0xf6, 0x28, 0), Star(0x4f, 0xf7, 0x2b, 0),
    Star(0xfb, 0xff, 0x2a, 0),

    Star(0xfe, 0x04, 0x3d, 1), Star(0xc4, 0x06, 0x10, 1),
    Star(0x1e, 0x07, 0x2d, 1), Star(0x83, 0x0b, 0x1f, 1),
    Star(0x2e, 0x0d, 0x3c, 1), Star(0x1f, 0x0e, 0x00, 1),
    Star(0xd8, 0x0e, 0x2c, 1), Star(0x03, 0x0f, 0x17, 1),
    Star(0x95, 0x11, 0x3f, 1), Star(0x6a, 0x17, 0x35, 1),
    Star(0xcc, 0x17, 0x02, 1), Star(0x00, 0x18, 0x32, 1),
    Star(0x92, 0x1d, 0x36, 1), Star(0xe3, 0x21, 0x04, 1),
    Star(0x2f, 0x2d, 0x37, 1), Star(0xf0, 0x2f, 0x0c, 1),
    Star(0x9b, 0x3e, 0x06, 1), Star(0xa4, 0x4c, 0x07, 1),
    Star(0xea, 0x4d, 0x13, 1), Star(0x84, 0x4e, 0x21, 1),
    Star(0x33, 0x52, 0x0f, 1), Star(0x70, 0x53, 0x0e, 1),
    Star(0x06, 0x59, 0x08, 1), Star(0x81, 0x60, 0x28, 1),
    Star(0x37, 0x61, 0x29, 1), Star(0x8f, 0x67, 0x2f, 1),
    Star(0x1b, 0x6a, 0x1d, 1), Star(0xbf, 0x7c, 0x12, 1),
    Star(0x51, 0x7f, 0x31, 1), Star(0x61, 0x86, 0x25, 1),
    Star(0x6a, 0x8f, 0x0d, 1), Star(0x6a, 0x91, 0x19, 1),
    Star(0x90, 0x92, 0x05, 1), Star(0x3b, 0x96, 0x24, 1),
    Star(0x8c, 0x97, 0x0a, 1), Star(0x06, 0x99, 0x03, 1),
    Star(0x38, 0x99, 0x38, 1), Star(0xa8, 0x99, 0x18, 1),
    Star(0x76, 0xa6, 0x20, 1), Star(0xad, 0xa6, 0x1c, 1),
    Star(0xec, 0xa6, 0x1e, 1), Star(0x86, 0xac, 0x15, 1),
    Star(0x78, 0xaf, 0x3e, 1), Star(0x7b, 0xb3, 0x09, 1),
    Star(0x27, 0xb8, 0x39, 1), Star(0x88, 0xc2, 0x23, 1),
    Star(0x44, 0xc3, 0x3a, 1), Star(0xcf, 0xc5, 0x34, 1),
    Star(0x35, 0xc9, 0x30, 1), Star(0x6e, 0xd1, 0x3b, 1),
    Star(0xd6, 0xd7, 0x16, 1), Star(0x3a, 0xd9, 0x2b, 1),
    Star(0xab, 0xe0, 0x11, 1), Star(0xe0, 0xe2, 0x1b, 1),
    Star(0x6f, 0xe6, 0x0b, 1), Star(0xb8, 0xe8, 0x14, 1),
    Star(0xd9, 0xe8, 0x1a, 1), Star(0xf9, 0xe8, 0x22, 1),
    Star(0x04, 0xf1, 0x2e, 1), Star(0x49, 0xf8, 0x26, 1),
    Star(0x10, 0xf9, 0x01, 1), Star(0x39, 0xfb, 0x33, 1),
    Star(0x28, 0xfc, 0x27, 1),

    Star(0xfa, 0x06, 0x19, 2), Star(0xe4, 0x07, 0x2d, 2),
    Star(0x72, 0x0a, 0x03, 2), Star(0x84, 0x1b, 0x00, 2),
    Star(0xba, 0x1d, 0x29, 2), Star(0xe3, 0x22, 0x04, 2),
    Star(0xd1, 0x26, 0x2a, 2), Star(0x89, 0x32, 0x30, 2),
    Star(0x5b, 0x36, 0x27, 2), Star(0x84, 0x3a, 0x36, 2),
    Star(0x53, 0x3f, 0x0d, 2), Star(0x08, 0x40, 0x1d, 2),
    Star(0x55, 0x40, 0x1a, 2), Star(0xaa, 0x41, 0x31, 2),
    Star(0xfb, 0x41, 0x2b, 2), Star(0xbc, 0x46, 0x16, 2),
    Star(0x93, 0x52, 0x39, 2), Star(0xb9, 0x57, 0x10, 2),
    Star(0x54, 0x59, 0x28, 2), Star(0xe6, 0x5a, 0x01, 2),
    Star(0xa7, 0x5d, 0x1b, 2), Star(0x2d, 0x5e, 0x35, 2),
    Star(0x14, 0x62, 0x21, 2), Star(0x69, 0x6d, 0x1f, 2),
    Star(0xce, 0x6f, 0x0b, 2), Star(0xdf, 0x75, 0x2f, 2),
    Star(0xcb, 0x77, 0x12, 2), Star(0x4e, 0x7c, 0x23, 2),
    Star(0x4a, 0x84, 0x0f, 2), Star(0x12, 0x86, 0x25, 2),
    Star(0x68, 0x8c, 0x32, 2), Star(0x03, 0x95, 0x20, 2),
    Star(0x0a, 0x9c, 0x17, 2), Star(0x5b, 0xa3, 0x08, 2),
    Star(0x5f, 0xa4, 0x3e, 2), Star(0x72, 0xa4, 0x2e, 2),
    Star(0xcc, 0xa6, 0x06, 2), Star(0x8a, 0xab, 0x0c, 2),
    Star(0xe0, 0xad, 0x26, 2), Star(0xf3, 0xaf, 0x0a, 2),
    Star(0x75, 0xb4, 0x13, 2), Star(0x68, 0xb7, 0x11, 2),
    Star(0x6d, 0xc2, 0x2c, 2), Star(0x76, 0xc3, 0x14, 2),
    Star(0xcf, 0xc4, 0x1e, 2), Star(0x04, 0xc5, 0x1c, 2),
    Star(0x13, 0xc6, 0x3f, 2), Star(0xb9, 0xc7, 0x3c, 2),
    Star(0x05, 0xd7, 0x34, 2), Star(0x95, 0xd7, 0x3a, 2),
    Star(0xfc, 0xd8, 0x02, 2), Star(0xe7, 0xdc, 0x09, 2),
    Star(0x1d, 0xe1, 0x05, 2), Star(0x05, 0xe6, 0x33, 2),
    Star(0x1c, 0xe9, 0x3b, 2), Star(0xa2, 0xed, 0x37, 2),
    Star(0x28, 0xee, 0x07, 2), Star(0xdd, 0xef, 0x18, 2),
    Star(0x6d, 0xf0, 0x38, 2), Star(0xa1, 0xf2, 0x0e, 2),
    Star(0x74, 0xf7, 0x3d, 2), Star(0x69, 0xf9, 0x22, 2),
    Star(0x3f, 0xff, 0x24, 2),

    Star(0x71, 0x10, 0x34, 3), Star(0xaf, 0x11, 0x23, 3),
    Star(0xa0, 0x14, 0x26, 3), Star(0x02, 0x17, 0x02, 3),
    Star(0x4b, 0x19, 0x31, 3), Star(0x93, 0x1c, 0x0e, 3),
    Star(0x1b, 0x1e, 0x25, 3), Star(0x32, 0x20, 0x2e, 3),
    Star(0xee, 0x20, 0x3a, 3), Star(0x79, 0x22, 0x2f, 3),
    Star(0x6c, 0x23, 0x17, 3), Star(0xbc, 0x25, 0x11, 3),
    Star(0x41, 0x29, 0x30, 3), Star(0x1c, 0x2e, 0x32, 3),
    Star(0xb9, 0x31, 0x01, 3), Star(0x83, 0x32, 0x05, 3),
    Star(0x95, 0x3a, 0x12, 3), Star(0x0d, 0x3f, 0x07, 3),
    Star(0x20, 0x41, 0x33, 3), Star(0x92, 0x45, 0x2c, 3),
    Star(0xd4, 0x47, 0x08, 3), Star(0xa1, 0x4b, 0x2d, 3),
    Star(0xd2, 0x4b, 0x3b, 3), Star(0xd6, 0x52, 0x24, 3),
    Star(0x9a, 0x5f, 0x1c, 3), Star(0x16, 0x60, 0x3d, 3),
    Star(0x1a, 0x63, 0x1f, 3), Star(0xcd, 0x66, 0x28, 3),
    Star(0xff, 0x67, 0x10, 3), Star(0x35, 0x69, 0x20, 3),
    Star(0x8f, 0x6c, 0x04, 3), Star(0xca, 0x6c, 0x2a, 3),
    Star(0x5a, 0x74, 0x09, 3), Star(0x60, 0x78, 0x38, 3),
    Star(0x72, 0x79, 0x1e, 3), Star(0x37, 0x7f, 0x29, 3),
    Star(0x12, 0x80, 0x14, 3), Star(0x29, 0x82, 0x2b, 3),
    Star(0x84, 0x98, 0x36, 3), Star(0x32, 0x99, 0x37, 3),
    Star(0xbb, 0xa0, 0x19, 3), Star(0x3e, 0xa3, 0x3e, 3),
    Star(0x4a, 0xa6, 0x1a, 3), Star(0x29, 0xa7, 0x21, 3),
    Star(0x9d, 0xb7, 0x22, 3), Star(0x6c, 0xb9, 0x15, 3),
    Star(0x0c, 0xc0, 0x0a, 3), Star(0xc2, 0xc3, 0x0f, 3),
    Star(0x2f, 0xc9, 0x0d, 3), Star(0xd2, 0xce, 0x16, 3),
    Star(0xf3, 0xce, 0x0b, 3), Star(0x75, 0xcf, 0x27, 3),
    Star(0x1a, 0xd5, 0x35, 3), Star(0x26, 0xd6, 0x39, 3),
    Star(0x80, 0xda, 0x3c, 3), Star(0xa9, 0xdd, 0x00, 3),
    Star(0xbc, 0xeb, 0x03, 3), Star(0x32, 0xef, 0x1b, 3),
    Star(0x67, 0xf0, 0x3f, 3), Star(0xef, 0xf1, 0x18, 3),
    Star(0xa8, 0xf3, 0x0c, 3), Star(0xde, 0xf9, 0x1d, 3),
    Star(0x2c, 0xfa, 0x13, 3)};

Starfield::Starfield() : _starsScrollX(368)
{
    for (size_t i = 0; i < 6; i++)
    {
        _control[i] = 0;
    }

    static const uint8_t buffer[] = {0x00, 0x47, 0x97, 0xde};
    for (size_t i = 0; i < PaletteSize; i++)
    {
        uint8_t r = buffer[i & 3];
        uint8_t g = buffer[(i >> 2) & 3];
        uint8_t b = buffer[(i >> 4) & 3];
        palette[i] = (0xff000000 | ((uint32_t)(r << 0x10)) |
                      ((uint32_t)(g << 0x08)) | b);
    }
}

void Starfield::DrawStars(Frame& frame)
{
    const size_t StarSeedTabSize = sizeof(starSeedTab) / sizeof(Star);

    if ((_control[5] & 1) != 0)
    {
        int num = _control[3] & 1;
        int num2 = (_control[4] & 1) | 2;
        for (size_t i = 0; i < StarSeedTabSize; i++)
        {
            if ((num == starSeedTab[i].set) || (num2 == starSeedTab[i].set))
            {
                int x = ((starSeedTab[i].x + _starsScrollX) % 0x100) +
                        0x10;  // if screen not flipped, 0x70
                int y = (0x70 + starSeedTab[i].y) % 0x100;
                if (((x >= 0) && (x < frame.Width)) &&
                    ((y >= 0) && (y < frame.Height)))
                {
                    frame.SetPixel(x, y, palette[starSeedTab[i].col]);
                }
            }
        }
    }
    Move();
}

void Starfield::Move()
{
    static const int speeds[] = {-1, -2, -3, 0, 3, 2, 1, 0};
    int num =
        (_control[0] & 1) | ((_control[1] & 1) << 1) | ((_control[2] & 1) << 2);
    _starsScrollX += speeds[num];
}

}  // namespace Galaga
