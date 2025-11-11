#include "BMFParser.hpp"

#include "mpc_fs.hpp"

#include "bitmap.hpp"

#include <Logger.hpp>

#include <cstdio>
#include <string>

using namespace mpc::lcdgui;

BMFParser::BMFParser(char *fntData, int fntSize, char *bmpData, int bmpSize)
{
    if (GetBMFontData(fntData, fntSize, &loadedFont))
    {
        MLOG("Loaded BMFont data correctly.\n");
    }

    Bitmap image;
    std::vector<std::vector<Pixel>> bmp;

    image.openFromData(bmpData, bmpSize);

    const bool bmpValid = image.isImage();

    if (bmpValid)
    {
        bmp = image.toPixelMatrix();
    }

    for (auto &row : bmp)
    {
        std::vector<bool> boolRow;

        for (const auto &column : row)
        {
            boolRow.push_back(!column.on);
        }

        atlas.push_back(boolRow);
    }
}

void BMFParser::OrderCharsByID(std::vector<bmfont_char> *chars) const
{
    std::vector<bmfont_char> result(255);
    for (int i = 0; i < chars->size(); i++)
    {
        const auto destid = chars->at(i).id;
        result[destid] = chars->at(i);
    }
    chars->clear();
    for (int i = 0; i < result.size(); i++)
    {
        chars->push_back(result[i]);
    }
}

bool BMFParser::GetBMFontData(const char *pBinary, size_t fileSize,
                              bmfont *pBMFont)
{
    if (!((pBinary[0] == 'B' && pBinary[1] == 'M' && pBinary[2] == 'F' &&
           pBinary[3] == 3)))
    {
        return false;
    }

    bmfont_stream stream((uint8_t *)pBinary, fileSize);
    stream.offsetBy(4);
    while (!stream.isEOF())
    {
        const uint8_t blockID = stream.getU8();
        MLOG("Parsing font blockID " + std::to_string(blockID));
        const int32_t blockSize = stream.getU32();
        switch (blockID)
        {
            case BMFONT_BLOCK_TYPE_INFO:
            {
                pBMFont->info.fontSize = stream.getS16();
                pBMFont->info.bitField = stream.getU8();
                pBMFont->info.charSet = stream.getU8();
                pBMFont->info.stretchH = stream.getU16();
                pBMFont->info.aa = stream.getU8();
                pBMFont->info.paddingUp = stream.getU8();
                pBMFont->info.paddingRight = stream.getU8();
                pBMFont->info.paddingDown = stream.getU8();
                pBMFont->info.paddingLeft = stream.getU8();
                pBMFont->info.spacingHoriz = stream.getU8();
                pBMFont->info.spacingVert = stream.getU8();
                pBMFont->info.outline = stream.getU8();
                const char *name = (const char *)stream.getPtr();
                stream.offsetBy(strlen(name) + 1);
                break;
            }
            case BMFONT_BLOCK_TYPE_COMMON:
            {
                pBMFont->common.lineHeight = stream.getU16();
                pBMFont->common.base = stream.getU16();
                pBMFont->common.scaleW = stream.getU16();
                pBMFont->common.scaleH = stream.getU16();
                pBMFont->common.pages = stream.getU16();
                pBMFont->common.bitField = stream.getU8();
                pBMFont->common.alphaChnl = stream.getU8();
                pBMFont->common.redChnl = stream.getU8();
                pBMFont->common.greenChnl = stream.getU8();
                pBMFont->common.blueChnl = stream.getU8();
                break;
            }
            case BMFONT_BLOCK_TYPE_PAGES:
            {
                const uint16_t pageCount = pBMFont->common.pages;
                for (uint16_t index = 0; index < pageCount; ++index)
                {
                    const char *pageName = (const char *)stream.getPtr();
                    uint8_t len = 0;
                    while (pageName[len] != 0)
                    {
                        pBMFont->pages[index].name[len] = pageName[len];
                        len++;
                    }
                    pBMFont->pages[index].length = len;
                    stream.offsetBy(len + 1);
                }
                break;
            }
            case BMFONT_BLOCK_TYPE_CHARS:
            {
                const int32_t charCount = blockSize / 20;
                if (charCount > BMFONT_MAX_CHARS)
                {
                    return false;
                }

                for (int32_t index = 0; index < charCount; ++index)
                {
                    bmfont_char foo;
                    pBMFont->chars.push_back(foo);
                    pBMFont->chars[index].id = stream.getU32();
                    pBMFont->chars[index].x = stream.getU16();
                    pBMFont->chars[index].y = stream.getU16();
                    pBMFont->chars[index].width = stream.getU16();
                    pBMFont->chars[index].height = stream.getU16();
                    pBMFont->chars[index].xoffset = stream.getS16();
                    pBMFont->chars[index].yoffset = stream.getS16();
                    pBMFont->chars[index].xadvance = stream.getS16();
                    pBMFont->chars[index].page = stream.getU8();
                    pBMFont->chars[index].chnl = stream.getU8();
                }

                OrderCharsByID(&pBMFont->chars);

                return true;
            }
            default:
                return false;
        }
    }
    return true;
}

bmfont BMFParser::getLoadedFont()
{
    return loadedFont;
}

std::vector<std::vector<bool>> BMFParser::getAtlas()
{
    return atlas;
}
