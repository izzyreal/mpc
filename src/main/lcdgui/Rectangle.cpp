#include "Rectangle.hpp"
#include <Paths.hpp>

#include <file/FileUtil.hpp>

using namespace mpc::lcdgui;
using namespace std;

Rectangle::Rectangle(const string& name, MRECT rect)
	: Component(name)
{
	w = rect.W();
	h = rect.H();
	x = rect.L;
	y = rect.T;
}

void Rectangle::Draw(std::vector< std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
		return;

	for (int x1 = x; x1 < x + w; x1++)
	{
		for (int y1 = y; y1 < y + h; y1++)
			(*pixels)[x1][y1] = on;
	}

	Component::Draw(pixels);
}

void Rectangle::setOn(bool on)
{
	if (this->on == on)
		return;

	this->on = on;

	SetDirty(true);
}
