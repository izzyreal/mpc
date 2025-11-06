#include "Background.hpp"

#include "MpcResourceUtil.hpp"

#include "lodepng.h"

using namespace mpc::lcdgui;

Background::Background() : Component("background")
{
    x = 0;
    y = 0;
    w = 248;
    h = 60;
}

void Background::repaintUnobtrusive(MRECT rect)
{
    unobtrusiveRect = unobtrusiveRect.Union(&rect);
    SetDirty();
}

void Background::setBackgroundName(const std::string &backgroundNameToUse)
{
    backgroundName = backgroundNameToUse;
    SetDirty();
}

void Background::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    if (dirty)
    {
        const std::string fileName = "screens/bg/" + backgroundName + ".png";

        unsigned int width = 248;
        unsigned int height = 60;

        if (backgroundName == "jd")
        {
            height = 360;
        }

        const auto file_data_char =
            MpcResourceUtil::get_resource_data(fileName);
        const std::vector<unsigned char> file_data(file_data_char.begin(),
                                                   file_data_char.end());
        std::vector<unsigned char> data;

        lodepng::decode(data, width, height, file_data, LCT_RGB, 8);

        if (backgroundName == "jd")
        {
            height = 60;
        }

        const bool unobtrusive = !unobtrusiveRect.Empty();

        int byteCounter(0);

        const int byteCounterOffset = (scrollOffset * width) * 3;

        byteCounter += byteCounterOffset;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (unobtrusive)
                {
                    if (x < unobtrusiveRect.L || x > unobtrusiveRect.R ||
                        y < unobtrusiveRect.T || y > unobtrusiveRect.B)
                    {
                        byteCounter += 3;
                        continue;
                    }
                }

                if (data[byteCounter] == 0)
                {
                    (*pixels)[x][y] = true;
                }
                else if (data[byteCounter] == 255)
                {
                    (*pixels)[x][y] = false;
                }
                byteCounter += 3;
            }
        }

        // clear bottom in case height != full LCD height
        if (backgroundName != "popup")
        {
            for (int y = height; y < 60; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    (*pixels)[x][y] = false;
                }
            }
        }

        unobtrusiveRect.Clear();
    }

    Component::Draw(pixels);
}

void Background::setScrolling(bool b)
{
    scrolling = b;
    scrollOffset = 0;

    if (!scrolling)
    {
        scrollingDown = false;
        while (!scrollThread->joinable())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        scrollThread->join();
    }
    else
    {
        scrollingDown = true;
        scrollThread = std::make_unique<std::thread>(
            [&]
            {
                while (scrolling)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));

                    if (scrollingDown)
                    {
                        scrollOffset += 1;
                    }
                    else if (scrolling)
                    {
                        scrollOffset -= 1;
                    }

                    if (scrollOffset >= 300)
                    {
                        scrollingDown = false;
                    }
                    else if (scrollOffset <= 0)
                    {
                        scrollingDown = true;
                    }

                    SetDirty();
                }
            });
    }
}
