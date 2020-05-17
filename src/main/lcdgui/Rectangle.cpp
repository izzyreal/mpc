#include "Rectangle.hpp"
#include <Paths.hpp>

#include <file/FileUtil.hpp>

using namespace mpc::lcdgui;
using namespace std;

Rectangle::Rectangle(MRECT rect)
	: Component("rectangle")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void Rectangle::Draw(std::vector< std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	for (int x1 = x; x1 < x + w; x1++)
	{
		for (int y1 = y; y1 < y + h; y1++)
		{
			(*pixels)[x1][y1] = on;
		}
	}

	dirty = false;
}

void Rectangle::setOn(bool on)
{
	if (this->on == on)
	{
		return;
	}

	this->on = on;

	dirty = true;
}
