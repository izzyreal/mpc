#include <lcdgui/screens/SndParamsScreen.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <controls/Controls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>

#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

SndParamsScreen::SndParamsScreen(const int layerIndex) 
	: ScreenComponent("params", layerIndex)
{
}

void SndParamsScreen::open()
{
	displaySnd();
	displayPlayX();
	displayLevel();
	displayTune();
	displayBeat();
	displaySampleAndNewTempo();
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
		sampler.lock()->sort();
		break;
	case 4:
	{
		if (sampler.lock()->getSoundCount() == 0)
		{
			return;
		}

		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		auto editSoundGui = mpc.getUis().lock()->getEditSoundGui();
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("trim");
		ls.lock()->openScreen("editsound");
		break;
	}
	case 5:
	{
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);
		sampler.lock()->playX();
		break;
	}
	}
}

void SndParamsScreen::turnWheel(int i)
{
	init();
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto sound = sampler.lock()->getSound().lock();

	if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
		displayPlayX();
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
		displayBeat();
		displayLevel();
		displaySampleAndNewTempo();
		displaySnd();
		displayTune();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
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
	}
	else if (param.compare("beat") == 0)
	{
		sound->setNumberOfBeats(sound->getBeatCount() + i);
		displayBeat();
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

	if (!sound)
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

	if (bpm < 300 || bpm > 3000)
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
	
	if (newBpm < 300 || newBpm > 3000)
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

	auto ls = Mpc::instance().getLayeredScreen().lock();

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
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("playx").lock()->setText(playXNames[soundGui->getPlayX()]);
}
