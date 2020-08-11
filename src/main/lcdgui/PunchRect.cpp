#include "HorizontalBar.hpp"

#include <cmath>

#include <Mpc.hpp>

using namespace mpc::lcdgui;

HorizontalBar::HorizontalBar(MRECT rect, int value)
	: Component("horizontal-bar")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
	this->value = value;
}

void HorizontalBar::setValue(int value)
{
	this->value = value;
	SetDirty();
}

void HorizontalBar::Draw(std::vector<std::vector<bool>>* pixels) {
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();
	int x = rect.L;
	int y = rect.T;
	int x2 = x + ((int)(floor((value - 1) / 2.55))) + 1;
	MRECT r(x, y, x2, y + 6);
	for (int i = rect.L; i <= rect.R; i++) {
		if (i > 247) break;
		for (int j = y; j < y + 5; j++) {
			(*pixels)[i][j] = false;
		}
	}
	for (int i = x; i < x2; i++) {
		if (i > rect.R) break;
		for (int j = y; j < y + 5; j++) {
			(*pixels)[i][j] = true;
		}
	}
	dirty = false;
}

HorizontalBar::~HorizontalBar() {
}
