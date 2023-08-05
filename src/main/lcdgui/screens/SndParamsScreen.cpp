#include "SndParamsScreen.hpp"

#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

SndParamsScreen::SndParamsScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "params", layerIndex)
{
}

void SndParamsScreen::open()
{
	auto sound = sampler->getSound() ? true : false;

	findField("snd")->setFocusable(sound);
	findField("playx")->setFocusable(sound);
	findField("level")->setFocusable(sound);
	findField("tune")->setFocusable(sound);
	findField("beat")->setFocusable(sound);
	findField("dummy")->setFocusable(!sound);

	displaySnd();
	displayPlayX();
	displayLevel();
	displayTune();
	displayBeat();
	displaySampleAndNewTempo();

	ls->setFunctionKeysArrangement(sound ? 1 : 0);
}

void SndParamsScreen::openWindow()
{
	init();

	if (param == "snd")
	{
		sampler->setPreviousScreenName("params");
		openScreen("sound");
	}
}

void SndParamsScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		openScreen("trim");
		break;
	case 1:
		openScreen("loop");
		break;
	case 2:
		openScreen("zone");
		break;
	case 3:
	{
        sampler->switchToNextSoundSortType();
		openScreen("popup");
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Sorting by " + sampler->getSoundSortingTypeName());
		popupScreen->returnToScreenAfterMilliSeconds("params", 200);
		break;
	}
	case 4:
	{
		if (sampler->getSoundCount() == 0)
			return;

		auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
		editSoundScreen->setReturnToScreenName("trim");
		openScreen("edit-sound");
		break;
	}
	case 5:
	{
		if (mpc.getControls()->isF6Pressed())
			return;

		mpc.getControls()->setF6Pressed(true);
		sampler->playX();
		break;
	}
	}
}

void SndParamsScreen::turnWheel(int i)
{
	init();
	auto sound = sampler->getSound();

	if (param == "playx")
	{
		sampler->setPlayX(sampler->getPlayX() + i);
		displayPlayX();
	}
	else if (param == "snd" && i > 0)
	{
		sampler->selectNextSound();
		displayBeat();
		displayLevel();
		displaySampleAndNewTempo();
		displaySnd();
		displayTune();
	}
	else if (param == "snd" && i < 0)
	{
		sampler->selectPreviousSound();
		displayBeat();
		displayLevel();
		displaySampleAndNewTempo();
		displaySnd();
		displayTune();
	}
	else if (param == "level")
	{
		sound->setLevel(sound->getSndLevel() + i);
		displayLevel();
	}
	else if (param == "tune")
	{
		sound->setTune(sound->getTune() + i);
		displayTune();
		displaySampleAndNewTempo();
	}
	else if (param == "beat")
	{
		sound->setBeatCount(sound->getBeatCount() + i);
		displayBeat();
		displaySampleAndNewTempo();
	}
}

void SndParamsScreen::displayLevel()
{
	auto sound = sampler->getSound();

	if (sound)
		findField("level")->setText(std::to_string(sound->getSndLevel()));
	else
		findField("level")->setText("100");
}

void SndParamsScreen::displayTune()
{
	auto sound = sampler->getSound();

	if (sound)
		findField("tune")->setText(std::to_string(sound->getTune()));
	else
		findField("tune")->setText("0");
}

void SndParamsScreen::displayBeat()
{
	auto sound = sampler->getSound();

	if (sound)
		findField("beat")->setText(std::to_string(sound->getBeatCount()));
	else
		findField("beat")->setText("4");
}

void SndParamsScreen::displaySampleAndNewTempo()
{
	auto sound = sampler->getSound();

	if (!sound || !sound->isLoopEnabled())
	{
		findLabel("sample-tempo")->setText("");
		findLabel("new-tempo")->setText("");
		return;
	}

	auto length = sound->getEnd() - sound->getLoopTo();
	auto lengthMs = (float)(length / (sound->getSampleRate() * 0.001));
	
	auto bpm = (int)(600000.0 / (lengthMs / sound->getBeatCount()));
	auto bpmString = std::to_string(bpm);
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

	findLabel("sample-tempo")->setText("Sample tempo=" + bpmString);

	auto newBpm = (int)(pow(2.0, (sound->getTune() / 120.0)) * bpm);
	bpmString = std::to_string(newBpm);
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

	findLabel("new-tempo")->setText("New tempo=" + bpmString);
}

void SndParamsScreen::displaySnd()
{
	auto sound = sampler->getSound();

	if (sampler->getSoundCount() == 0)
	{
		findField("snd")->setText("(no sound)");
		ls->setFocus("dummy");
		return;
	}

	if (ls->getFocus() == "dummy")
		ls->setFocus("snd");

	auto sampleName = sound->getName();

	if (!sound->isMono())
		sampleName = StrUtil::padRight(sampleName, " ", 16) + "(ST)";

	findField("snd")->setText(sampleName);

}

void SndParamsScreen::displayPlayX()
{
	findField("playx")->setText(playXNames[sampler->getPlayX()]);
}
