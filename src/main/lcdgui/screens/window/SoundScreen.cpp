#include "SoundScreen.hpp"

#include <ui/NameGui.hpp>
#include <ui/sampler/SoundGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

SoundScreen::SoundScreen(const int layerIndex) 
	: ScreenComponent("soundscreen", layerIndex)
{
}

void SoundScreen::open()
{
	displaySoundName();
	displayType();
	displayRate();
	displaySize();
}

void SoundScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("soundname") == 0)
	{
		auto nameGui = mpc.getUis().lock()->getNameGui();
		nameGui->setName(ls.lock()->lookupField("soundname").lock()->getText());
		nameGui->setParameterName("soundname");
		ls.lock()->openScreen("name");
	}
}

void SoundScreen::function(int i)
{
	BaseControls::function(i);

	switch (i)
	{
	case 1:
		ls.lock()->openScreen("deletesound");
		break;
	case 2:
		ls.lock()->openScreen("convertsound");
		break;
	case 4:
		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		ls.lock()->openScreen("copysound");
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->setNewName(newSampleName);
		break;
	}
}

void SoundScreen::displaySoundName()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findField("soundname").lock()->setText("");
		return;
	}

	findField("soundname").lock()->setText(sound->getName());
}

void SoundScreen::displayType()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("type").lock()->setText("");
		return;
	}

	findLabel("type").lock()->setText("Type:" + string(sound->isMono() ? "MONO" : "STEREO"));
}

void SoundScreen::displayRate()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("rate").lock()->setText("");
		return;
	}

	findLabel("rate").lock()->setText("Rate: " + to_string(sound->getSampleRate()) + "Hz");
}

void SoundScreen::displaySize()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("size").lock()->setText("");
		return;
	}

	findLabel("size").lock()->setText("Size:" + StrUtil::padLeft(to_string(sound->getSampleData()->size() / 500), " ", 4) + "kbytes");
}
