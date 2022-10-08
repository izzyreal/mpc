#include <lcdgui/MixerFader.hpp>

//#include <Logger.hpp>

using namespace mpc::lcdgui;

MixerFader::MixerFader(MRECT rect)
	: Component("mixer-fader")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void MixerFader::setValue(int value)
{
    this->value = value;
	SetDirty();
}

void MixerFader::setColor(bool on)
{
    this->color = on;
	SetDirty();
}

void MixerFader::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();
	
	for (int i = rect.L; i < rect.R; i++)
	{
		for (int j = rect.T; j < rect.B; j++)
		{
			(*pixels)[i][j] = !color;
		}
	}

	auto barHeight = (value / 100.0) * h;

	MRECT tmp(rect.L, rect.B - barHeight, rect.R, rect.B);
	
	for (int i = tmp.L; i < tmp.R; i++)
	{
		for (int j = tmp.T; j < tmp.B; j++)
		{
			(*pixels)[i][j] = color;
		}
	}
	dirty = false;
}
