#pragma once

#include <cstddef>
#include <vector>

namespace mpc::lcdgui
{
#define BMFONT_BLOCK_TYPE_INFO 1
#define BMFONT_BLOCK_TYPE_COMMON 2
#define BMFONT_BLOCK_TYPE_PAGES 3
#define BMFONT_BLOCK_TYPE_CHARS 4
#define BMFONT_MAX_PAGES 2
#define BMFONT_MAX_CHARS 255

    struct bmfont_info
    {
        int16_t fontSize;
        uint8_t bitField;
        uint8_t charSet;
        uint16_t stretchH;
        uint8_t aa;
        uint8_t paddingUp;
        uint8_t paddingRight;
        uint8_t paddingDown;
        uint8_t paddingLeft;
        uint8_t spacingHoriz;
        uint8_t spacingVert;
        uint8_t outline;
    };

    struct bmfont_common
    {
        uint16_t lineHeight;
        uint16_t base;
        uint16_t scaleW;
        uint16_t scaleH;
        uint16_t pages;
        uint8_t bitField;
        uint8_t alphaChnl;
        uint8_t redChnl;
        uint8_t greenChnl;
        uint8_t blueChnl;
    };
    struct bmfont_char
    {
        uint32_t id;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        int16_t xoffset;
        int16_t yoffset;
        int16_t xadvance;
        uint8_t page;
        uint8_t chnl;
    };

    struct bmfont_file
    {
        bmfont_info *pInfo;
        const char *pFontName;
        bmfont_common *pCommon;
        const char *pages[BMFONT_MAX_PAGES];
        std::vector<bmfont_char> chars;
    };
    struct bmfont_stream
    {
        bmfont_stream(uint8_t *pBuffer, size_t _size)
            : pos(0), size(_size), pByteData(pBuffer)
        {
        }
        // Unsigned
        uint8_t currentU8() const
        {
            return (uint8_t)pByteData[pos];
        }
        uint8_t getU8()
        {
            const uint8_t value = currentU8();
            pos += 1;
            return value;
        }
        uint16_t currentU16() const
        {
            return (uint16_t)((pByteData[pos + 1] << 8) | pByteData[pos]);
        }
        uint16_t getU16()
        {
            const uint16_t value = currentU16();
            pos += 2;
            return value;
        }
        uint32_t currentU32() const
        {
            return (uint32_t)((pByteData[pos + 3] << 24) |
                              (pByteData[pos + 2] << 16) |
                              (pByteData[pos + 1] << 8) | pByteData[pos]);
        }
        uint32_t getU32()
        {
            const uint32_t value = currentU32();
            pos += 4;
            return value;
        }
        // Signed
        int8_t currentS8() const
        {
            return (int8_t)pByteData[pos];
        }
        int8_t getS8()
        {
            const int8_t value = static_cast<int8_t>(currentU8());
            pos += 1;
            return value;
        }
        int16_t currentS16() const
        {
            return (int16_t)((pByteData[pos + 1] << 8) | pByteData[pos + 0]);
        }
        int16_t getS16()
        {
            const int16_t value = static_cast<int16_t>(currentU16());
            pos += 2;
            return value;
        }
        int32_t currentS32() const
        {
            return (int32_t)((pByteData[pos + 3] << 24) |
                             (pByteData[pos + 2] << 16) |
                             (pByteData[pos + 1] << 8) | pByteData[pos]);
        }
        int32_t getS32()
        {
            const int32_t value = static_cast<int32_t>(currentU32());
            pos += 4;
            return value;
        }
        uint8_t *getPtr() const
        {
            return &pByteData[pos];
        }
        void offsetBy(size_t byteCount)
        {
            pos += byteCount;
        }
        bool isEOF() const
        {
            return pos >= size;
        }

    private:
        size_t pos;
        size_t size;
        uint8_t *pByteData;
    };

    struct bmfont_page
    {
        char name[32];
        uint8_t length;
    };
    struct bmfont
    {
        bmfont_info info;
        bmfont_common common;
        bmfont_page pages[BMFONT_MAX_PAGES];
        std::vector<bmfont_char> chars;
    };
} // namespace mpc::lcdgui
