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
	if (hidden || !IsDirty())
	{
		return;
	}

	auto rect = getRect();

	int valuePixels = (int)((value - 2) / 3);
	for (int i = rect.L; i < rect.L + 4; i++) {
		for (int j = rect.T; j < rect.T + 32 - valuePixels; j++) {
			(*pixels)[i][j] = !color;
		}
	}
	MRECT tmp(rect.L, rect.T + 32 - valuePixels, rect.L + 3, rect.T + 32);
	for (int i = tmp.L; i < tmp.R + 1; i++) {
		for (int j = tmp.T; j < tmp.B + 1; j++) {
			(*pixels)[i][j] = color;
		}
	}
	dirty = false;
}

VerticalBar::~VerticalBar() {
}

