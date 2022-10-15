#include "SoundMemoryScreen.hpp"

#include <lcdgui/PunchRect.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

SoundMemoryScreen::SoundMemoryScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "sound-memory", layerIndex)
{
	MRECT r(23, 26, 223, 35);
	addChildT<PunchRect>("free-memory", r).lock()->setOn(true);
}

void SoundMemoryScreen::open()
{
	displayFreeMemoryTime();
	displayIndicator();
	displayMegabytesInstalled();
}

void SoundMemoryScreen::displayFreeMemoryTime()
{
	auto total = 33374880;
	
	for (auto& s : sampler->getSounds())
		total -= (s.lock()->getSampleData()->size() * 2);

	auto time = StrUtil::padLeft(StrUtil::TrimDecimals(total / 176400.0, 1), " ", 6);
	findLabel("free-memory-time").lock()->setText("Free memory(time):" + time);
}

void SoundMemoryScreen::displayIndicator()
{
	auto free = (32620.0 - sampler->getFreeSampleSpace()) / 32620.0;
	findChild<PunchRect>("free-memory").lock()->setSize((int)floor(200.0 * free), 9);
}

void SoundMemoryScreen::displayMegabytesInstalled()
{
	findLabel("megabytes-installed").lock()->setText("32");
}
