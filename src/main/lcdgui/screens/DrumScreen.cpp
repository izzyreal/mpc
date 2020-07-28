#include "DrumScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

DrumScreen::DrumScreen(const int layerIndex) 
	: ScreenComponent("drum", layerIndex)
{
}

void DrumScreen::open()
{
	findField("program-change").lock()->setAlignment(Alignment::Centered);
	findField("midi-volume").lock()->setAlignment(Alignment::Centered);
	init();
	displayDrum();
	displayPadToInternalSound();
	displayPgm();
	displayPgmChange();
	displayMidiVolume();
	displayCurrentVal();
}

void DrumScreen::function(int f)
{
    init();
	
	switch (f)
	{
    case int (0):
        ls.lock()->openScreen("program-assign");
        break;
    case 1:
        ls.lock()->openScreen("program-params");
        break;
    case 2:
        ls.lock()->openScreen("select-drum");
        break;
    case int (3):
        ls.lock()->openScreen("purge");
        break;
    }

}

void DrumScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("drum") == 0)
	{
		setDrum(drum + i);
	}
	else if (param.compare("pgm") == 0)
	{
		mpcSoundPlayerChannel->setProgram(mpcSoundPlayerChannel->getProgram() + i);
		displayPgm();
	}
	else if (param.compare("program-change") == 0)
	{
		mpcSoundPlayerChannel->setReceivePgmChange(i > 0);
		displayPgmChange();
	}
	else if (param.compare("midi-volume") == 0)
	{
		mpcSoundPlayerChannel->setReceiveMidiVolume(i > 0);
		displayMidiVolume();
	}
	else if (param.compare("current-val") == 0)
	{
		mpcSoundPlayerChannel->setLastReceivedMidiVolume(mpcSoundPlayerChannel->getLastReceivedMidiVolume() + i);
		displayCurrentVal();
	}
	else if (param.compare("padtointernalsound") == 0)
	{
		setPadToIntSound(i > 0);
	}
}

void DrumScreen::displayCurrentVal()
{
	init();
	findField("current-val").lock()->setTextPadded(mpcSoundPlayerChannel->getLastReceivedMidiVolume());
}

void DrumScreen::displayDrum()
{
	findField("drum").lock()->setText(to_string(drum + 1));
}

void DrumScreen::displayPadToInternalSound()
{
	findField("padtointernalsound").lock()->setText(padToInternalSound ? "ON" : "OFF");
}

void DrumScreen::displayPgm()
{
	auto pn = mpcSoundPlayerChannel->getProgram();
	findField("pgm").lock()->setText(StrUtil::padLeft(to_string(pn + 1), " ", 2) + "-" + dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pn).lock())->getName());
}

void DrumScreen::displayPgmChange()
{
	findField("program-change").lock()->setText(mpcSoundPlayerChannel->receivesPgmChange() ? "RECEIVE" : "IGNORE");
}

void DrumScreen::displayMidiVolume()
{
	findField("midi-volume").lock()->setText(mpcSoundPlayerChannel->receivesMidiVolume() ? "RECEIVE" : "IGNORE");
}

bool DrumScreen::isPadToIntSound()
{
	return padToInternalSound;
}

void DrumScreen::setPadToIntSound(bool b)
{
	padToInternalSound = b;
	displayPadToInternalSound();
}

void DrumScreen::setDrum(int i)
{
	if (i < 0 || i > 3)
	{
		return;
	}

	drum = i;

	init();

	displayDrum();
	displayPgm();
	displayPgmChange();
	displayMidiVolume();
	displayCurrentVal();
}
