#include <lcdgui/VerticalBar.hpp>

#include <Logger.hpp>

using namespace mpc::lcdgui;

VerticalBar::VerticalBar(MRECT rect)
	: Component("vertical-bar")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void VerticalBar::setValue(int value)
{
    this->value = value;
	SetDirty();
}

void VerticalBar::setColor(bool on)
{
    this->color = on;
	SetDirty();
}

void VerticalBar::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();

	//int valuePixels = (int)((value - 2) / 3);
	
	for (int i = rect.L; i < rect.R; i++)
	{
		for (int j = rect.T; j < rect.B; j++)
		{
			(*pixels)[i][j] = color;
		}
	}
	
	/*
	MRECT tmp(rect.L, rect.B - valuePixels, rect.R - 1, rect.B);
	
	for (int i = tmp.L; i < tmp.R; i++)
	{
		for (int j = tmp.T; j < tmp.B + 1; j++)
		{
			//(*pixels)[i][j] = color;
		}
	}
	*/
	dirty = false;
}
