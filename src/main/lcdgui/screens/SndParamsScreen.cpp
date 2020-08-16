#include "SndParamsScreen.hpp"

#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;
using namespace std;

SndParamsScreen::SndParamsScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "params", layerIndex)
{
}

void SndParamsScreen::open()
{
	auto sound = sampler.lock()->getSound().lock() ? true : false;

	findField("snd").lock()->setFocusable(sound);
	findField("playx").lock()->setFocusable(sound);
	findField("level").lock()->setFocusable(sound);
	findField("tune").lock()->setFocusable(sound);
	findField("beat").lock()->setFocusable(sound);
	findField("dummy").lock()->setFocusable(!sound);

	displaySnd();
	displayPlayX();
	displayLevel();
	displayTune();
	displayBeat();
	displaySampleAndNewTempo();

	ls.lock()->setFunctionKeysArrangement(sound ? 1 : 0);
}

void SndParamsScreen::openWindow()
{
	init();

	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("params");
		ls.lock()->openScreen("sound");
	}
}

void SndParamsScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		ls.lock()->openScreen("trim");
		break;
	case 1:
		ls.lock()->openScreen("loop");
		break;
	case 2:
		ls.lock()->openScreen("zone");
		break;
	case 3:
	{
		sampler.lock()->sort();
		mpc.getLayeredScreen().lock()->openScreen("popup");
		auto popupScreen = dynamic_pointer_cast<PopupScreen>(mpc.screens->getScreenComponent("popup"));
		popupScreen->setText("Sorting by " + sampler.lock()->getSoundSortingTypeName());
		popupScreen->returnToScreenAfterMilliSeconds("params", 200);
		break;
	}
	case 4:
	{
		if (sampler.lock()->getSoundCount() == 0)
		{
			return;
		}

		auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(mpc.screens->getScreenComponent("edit-sound"));
		editSoundScreen->setReturnToScreenName("trim");
		ls.lock()->openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (mpc.getControls().lock()->isF6Pressed())
		{
			return;
		}

		mpc.getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
	}
}

void SndParamsScreen::turnWheel(int i)
{
	init();
	auto sound = sampler.lock()->getSound().lock();

	if (param.compare("playx") == 0)
	{
		sampler.lock()->setPlayX(sampler.lock()->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->selectNextSound();
		displayBeat();
		displayLevel();
		displaySampleAndNewTempo();
		displaySnd();
		displayTune();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->selectPreviousSound();
		displayBeat();
		displayLevel();
		displaySampleAndNewTempo();
		displaySnd();
		displayTune();
	}
	else if (param.compare("level") == 0)
	{
		sound->setLevel(sound->getSndLevel() + i);
		displayLevel();
	}
	else if (param.compare("tune") == 0)
	{
		sound->setTune(sound->getTune() + i);
		displayTune();
		displaySampleAndNewTempo();
	}
	else if (param.compare("beat") == 0)
	{
		sound->setBeatCount(sound->getBeatCount() + i);
		displayBeat();
		displaySampleAndNewTempo();
	}
}

void SndParamsScreen::displayLevel()
{
	auto sound = sampler.lock()->getSound().lock();

	if (sound)
	{
		findField("level").lock()->setText(to_string(sound->getSndLevel()));
	}
	else
	{
		findField("level").lock()->setText("100");
	}
}

void SndParamsScreen::displayTune()
{
	auto sound = sampler.lock()->getSound().lock();

	if (sound)
	{
		findField("tune").lock()->setText(to_string(sound->getTune()));
	}
	else
	{
		findField("tune").lock()->setText("0");
	}
}

void SndParamsScreen::displayBeat()
{
	auto sound = sampler.lock()->getSound().lock();

	if (sound)
	{
		findField("beat").lock()->setText(to_string(sound->getBeatCount()));
	}
	else
	{
		findField("beat").lock()->setText("4");
	}
}

void SndParamsScreen::displaySampleAndNewTempo()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound || !sound->isLoopEnabled())
	{
		findLabel("sample-tempo").lock()->setText("");
		findLabel("new-tempo").lock()->setText("");
		return;
	}

	auto length = sound->getEnd() - sound->getLoopTo();
	auto lengthMs = (float)(length / (sound->getSampleRate() * 0.001));
	
	auto bpm = (int)(600000.0 / (lengthMs / sound->getBeatCount()));
	auto bpmString = to_string(bpm);
	auto part1 = bpmString.substr(0, bpmString.length() - 1);
	auto part2 = bpmString.substr(bpmString.length() - 1);

	if (bpm < 300 || bpm > 9999)
	{
		part1 = "---";
		part2 = "-";
	}

	bpmString = part1 + "." + part2;
	bpmString = StrUtil::padLeft(bpmString, " ", 5);

	bpmString = mpc::Util::replaceDotWithSmallSpaceDot(bpmString);

	findLabel("sample-tempo").lock()->setText("Sample tempo=" + bpmString);

	auto newBpm = (int)(pow(2.0, (sound->getTune() / 120.0)) * bpm);
	bpmString = to_string(newBpm);
	part1 = bpmString.substr(0, bpmString.length() - 1);
	part2 = bpmString.substr(bpmString.length() - 1);
	
	if (newBpm < 300 || newBpm > 9999)
	{
		part1 = "---";
		part2 = "-";
	}

	bpmString = part1 + "." + part2;
	bpmString = StrUtil::padLeft(bpmString, " ", 5);

	bpmString = mpc::Util::replaceDotWithSmallSpaceDot(bpmString);

	findLabel("new-tempo").lock()->setText("New tempo=" + bpmString);
}

void SndParamsScreen::displaySnd()
{
	auto sound = sampler.lock()->getSound().lock();

	if (sampler.lock()->getSoundCount() == 0)
	{
		findField("snd").lock()->setText("(no sound)");
		//ls.lock()->setFocus("dummy"); // Is this required?
		return;
	}

	auto ls = mpc.getLayeredScreen().lock();

	if (ls->getFocus().compare("dummy") == 0)
	{
		ls->setFocus(findField("snd").lock()->getName());
	}

	auto sampleName = sound->getName();

	if (!sound->isMono())
	{
		sampleName = StrUtil::padRight(sampleName, " ", 16);
		sampleName += "(ST)";
	}

	findField("snd").lock()->setText(sampleName);

}

void SndParamsScreen::displayPlayX()
{
	findField("playx").lock()->setText(playXNames[sampler.lock()->getPlayX()]);
}
