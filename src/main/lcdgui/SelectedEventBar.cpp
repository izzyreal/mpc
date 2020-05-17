#include "SelectedEventBar.hpp"

using namespace mpc::lcdgui;

SelectedEventBar::SelectedEventBar(MRECT rect)
	: Component("selected-event-bar")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void SelectedEventBar::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();
	for (int i = 0; i < rect.W(); i++) {
		for (int j = 0; j < rect.H(); j++) {
			pixels->at(i + rect.L).at(j + rect.T) = true;
		}
	}
	dirty = false;
}

SelectedEventBar::~SelectedEventBar() {
}
