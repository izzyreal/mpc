#include "DrumScreen.hpp"

#include "SelectDrumScreen.hpp"

using namespace mpc::lcdgui::screens;

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
    case 0:
        openScreen("program-assign");
        break;
    case 1:
        openScreen("program-params");
        break;
    case 2:
	{
		auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>();
		selectDrumScreen->redirectScreen = "drum";
		openScreen("select-drum");
		break;
	}
    case 3:
        openScreen("purge");
        break;
    }

}

void DrumScreen::turnWheel(int i)
{
	init();
	
	if (param == "drum")
	{
		setDrum(drum + i);
	}
	else if (param == "pgm")
	{
        activeDrum().setProgram(activeDrum().getProgram() + i);
		displayPgm();
	}
	else if (param == "program-change")
	{
        activeDrum().setReceivePgmChange(i > 0);
		displayPgmChange();
	}
	else if (param == "midi-volume")
	{
        activeDrum().setReceiveMidiVolume(i > 0);
		displayMidiVolume();
	}
	else if (param == "current-val")
	{
        activeDrum().setLastReceivedMidiVolume(activeDrum().getLastReceivedMidiVolume() + i);
		displayCurrentVal();
	}
	else if (param == "padtointernalsound")
	{
		setPadToIntSound(i > 0);
	}
}

void DrumScreen::displayCurrentVal()
{
	init();
	findField("current-val")->setTextPadded(activeDrum().getLastReceivedMidiVolume());
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
	auto pn = activeDrum().getProgram();
	findField("pgm")->setText(StrUtil::padLeft(std::to_string(pn + 1), " ", 2) + "-" + sampler->getProgram(pn)->getName());
}

void DrumScreen::displayPgmChange()
{
	findField("program-change")->setText(activeDrum().receivesPgmChange() ? "RECEIVE" : "IGNORE");
}

void DrumScreen::displayMidiVolume()
{
	findField("midi-volume")->setText(activeDrum().receivesMidiVolume() ? "RECEIVE" : "IGNORE");
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

void DrumScreen::setDrum(unsigned char i)
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

unsigned char DrumScreen::getDrum()
{
    return drum;
}
