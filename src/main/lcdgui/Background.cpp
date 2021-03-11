#include "Background.hpp"
#include <Paths.hpp>

#include <file/FileUtil.hpp>

using namespace mpc::lcdgui;
using namespace std;

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

void Background::setName(const string& name)
{ 
	this->name = name;
	SetDirty();
}

void Background::Draw(vector<vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	if (dirty)
	{

		string backgroundPath = mpc::Paths::bgPath() + name + ".bmp";
		const int infosize = 54;

		FILE* f = moduru::file::FileUtil::fopenw(backgroundPath, "rb");

		if (f == NULL)
		{
			return;
		}

		unsigned char info[infosize];
		fread(info, sizeof(unsigned char), infosize, f); // read the 54-byte header
		int imageDataOffset = info[10];
		int width = info[18];
		int height = 256 - info[22];
		int imageSize = width * height;
		fseek(f, imageDataOffset, 0);

		vector<unsigned char> data(imageSize);
		fread(&data[0], sizeof(unsigned char), imageSize, f); // read the rest of the data at once
		fclose(f);
		int colorCount = (imageDataOffset - infosize) / 4;

		int byteCounter = 0;

		const auto unobtrusive = !unobtrusiveRect.Empty();

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
				{
					(*pixels)[x][y] = false;
				}
			}
		}

		unobtrusiveRect.Clear();
	}

	Component::Draw(pixels);
}
