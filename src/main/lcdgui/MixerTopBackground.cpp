#include <lcdgui/MixerTopBackground.hpp>

using namespace mpc::lcdgui;

MixerTopBackground::MixerTopBackground(MRECT rect)
	: Component("mixer-top-background")
{
	setSize(rect.W(), rect.H());
	setLocation(rect.L, rect.T);
}


void MixerTopBackground::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	auto rect = getRect();

	for (int i = rect.L; i < rect.R + 1; i++) {
		for (int j = rect.T; j < rect.B + 1; j++) {
			(*pixels)[i][j] = true;
		}
	}
	dirty = false;
}

MixerTopBackground::~MixerTopBackground() {
}
