#include <lcdgui/MixerFaderBackground.hpp>

using namespace mpc::lcdgui;

MixerFaderBackground::MixerFaderBackground(MRECT rect)
	: Component("mixer-fader-background")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}

void MixerFaderBackground::Draw(std::vector<std::vector<bool>>* pixels) {
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();

	for (int i = rect.L; i < rect.R; i++) {
		for (int j = rect.T; j < rect.B; j++) {
			(*pixels)[i][j] = true;
		}
	}

	dirty = false;
}

MixerFaderBackground::~MixerFaderBackground() {
}
