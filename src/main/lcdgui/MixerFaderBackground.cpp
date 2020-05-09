#include <lcdgui/MixerFaderBackground.hpp>

using namespace mpc::lcdgui;

MixerFaderBackground::MixerFaderBackground(MRECT rect)
	: Component("mixer-fader-background")
{
	this->rect = rect;
}

void MixerFaderBackground::Draw(std::vector<std::vector<bool>>* pixels) {
	if (IsHidden()) return;
	for (int i = rect.L; i < rect.R + 1; i++) {
		for (int j = rect.T; j < rect.B + 1; j++) {
			(*pixels)[i][j] = true;
		}
	}
	dirty = false;
}

MixerFaderBackground::~MixerFaderBackground() {
}
