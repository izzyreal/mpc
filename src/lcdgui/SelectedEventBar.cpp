#include "SelectedEventBar.hpp"

using namespace mpc::lcdgui;

SelectedEventBar::SelectedEventBar(MRECT rect)
{
	this->rect = rect;
}

void SelectedEventBar::Draw(std::vector<std::vector<bool> >* pixels)
{
	for (int i = 0; i < rect.W(); i++) {
		for (int j = 0; j < rect.H(); j++) {
			pixels->at(i + rect.L).at(j + rect.T) = true;
		}
	}
	dirty = false;
}

SelectedEventBar::~SelectedEventBar() {
}
