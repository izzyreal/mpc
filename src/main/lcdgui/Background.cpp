#include "Background.hpp"
#include <Paths.hpp>

#include <file/FileUtil.hpp>

#include <cmrc/cmrc.hpp>
#include <string_view>

CMRC_DECLARE(mpc);

#include <Logger.hpp>

using namespace mpc::lcdgui;

Background::Background()
	: Component("background")
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

void Background::setName(const std::string& name)
{ 
	this->name = name;
	SetDirty();
}

void Background::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	if (dirty)
	{
        auto fs = cmrc::mpc::get_filesystem();
        auto fileName = "screens/bg/" + name + ".bmp";
        
        if (!fs.exists(fileName))
        {
            MLOG("Background " + fileName + " does not exist!");
            Component::Draw(pixels);
            return;
        }
                
        auto file = fs.open("screens/bg/" + name + ".bmp");

        char* data = (char*) std::string_view(file.begin(), file.end() - file.begin()).data();

		int imageDataOffset = data[10];
		int width = (unsigned char) data[18];
		int height = 256 - (unsigned char)(data[22]);

		int colorCount = (imageDataOffset - 54) / 4;

		const bool unobtrusive = !unobtrusiveRect.Empty();

        int byteCounter = imageDataOffset;

        for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (unobtrusive)
				{
					if (x < unobtrusiveRect.L || x > unobtrusiveRect.R || y < unobtrusiveRect.T || y > unobtrusiveRect.B)
					{
						byteCounter++;
						continue;
					}
				}

				auto value = data[byteCounter++];

				if ((colorCount <= 2 && value == 1) || (colorCount > 2 && value == 2))
				{
					(*pixels)[x][y] = true;
				}
				else if (value == 0)
				{
					(*pixels)[x][y] = false;
				}
			}
		}

		// clear bottom in case height != full LCD height
		if (name.compare("popup") != 0)
		{
			for (int y = height; y < 60; y++)
			{
				for (int x = 0; x < width; x++)
					(*pixels)[x][y] = false;
			}
		}

		unobtrusiveRect.Clear();
	}

	Component::Draw(pixels);
}
