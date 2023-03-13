#include "DrumScreen.hpp"

#include "SelectDrumScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace moduru::lang;

DrumScreen::DrumScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "drum", layerIndex)
{
}

void DrumScreen::open()
{
	findField("program-change")->setAlignment(Alignment::Centered);
	findField("midi-volume")->setAlignment(Alignment::Centered);
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
        openScreen("program-assign");
        break;
    case 1:
        openScreen("program-params");
        break;
    case 2:
	{
		auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>("select-drum");
		selectDrumScreen->redirectScreen = "drum";
		openScreen("select-drum");
		break;
	}
    case int (3):
        openScreen("purge");
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
		mpcSoundPlayerChannel().setProgram(mpcSoundPlayerChannel().getProgram() + i);
		displayPgm();
	}
	else if (param.compare("program-change") == 0)
	{
		mpcSoundPlayerChannel().setReceivePgmChange(i > 0);
		displayPgmChange();
	}
	else if (param.compare("midi-volume") == 0)
	{
		mpcSoundPlayerChannel().setReceiveMidiVolume(i > 0);
		displayMidiVolume();
	}
	else if (param.compare("current-val") == 0)
	{
		mpcSoundPlayerChannel().setLastReceivedMidiVolume(mpcSoundPlayerChannel().getLastReceivedMidiVolume() + i);
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
	findField("current-val")->setTextPadded(mpcSoundPlayerChannel().getLastReceivedMidiVolume());
}

void DrumScreen::displayDrum()
{
	findField("drum")->setText(std::to_string(drum + 1));
}

void DrumScreen::displayPadToInternalSound()
{
	findField("padtointernalsound")->setText(padToInternalSound ? "ON" : "OFF");
}

void DrumScreen::displayPgm()
{
	auto pn = mpcSoundPlayerChannel().getProgram();
	findField("pgm")->setText(StrUtil::padLeft(std::to_string(pn + 1), " ", 2) + "-" + sampler->getProgram(pn)->getName());
}

void DrumScreen::displayPgmChange()
{
	findField("program-change")->setText(mpcSoundPlayerChannel().receivesPgmChange() ? "RECEIVE" : "IGNORE");
}

void DrumScreen::displayMidiVolume()
{
	findField("midi-volume")->setText(mpcSoundPlayerChannel().receivesMidiVolume() ? "RECEIVE" : "IGNORE");
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
