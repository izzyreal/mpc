#pragma once

#include <vector>

#include "BMFStructs.hpp"

namespace mpc::lcdgui
{
    class BMFParser
    {

    private:
        bmfont loadedFont;
        std::vector<std::vector<bool>> atlas;

    private:
        void OrderCharsByID(std::vector<bmfont_char> *chars);
        bool GetBMFontData(const char *pBinary, size_t fileSize,
                           bmfont *pBMFont);

    public:
        std::vector<std::vector<bool>> getAtlas();
        bmfont getLoadedFont();

    public:
        BMFParser(char *fntData, int fntSize, char *bmpData, int bmpSize);
    };
} // namespace mpc::lcdgui
