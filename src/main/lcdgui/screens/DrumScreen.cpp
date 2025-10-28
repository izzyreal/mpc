#include "DrumScreen.hpp"

#include "SelectDrumScreen.hpp"
#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens;

DrumScreen::DrumScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "drum", layerIndex)
{
}

void DrumScreen::open()
{
    findField("program-change")->setAlignment(Alignment::Centered);
    findField("midi-volume")->setAlignment(Alignment::Centered);
    displayDrum();
    displayPadToInternalSound();
    displayPgm();
    displayPgmChange();
    displayMidiVolume();
    displayCurrentVal();
}

void DrumScreen::function(int f)
{

    switch (f)
    {
        case 0:
            mpc.getLayeredScreen()->openScreen<PgmAssignScreen>();
            break;
        case 1:
            mpc.getLayeredScreen()->openScreen<PgmParamsScreen>();
            break;
        case 2:
        {
            auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "drum";
            mpc.getLayeredScreen()->openScreen<SelectDrumScreen>();
            break;
        }
        case 3:
            mpc.getLayeredScreen()->openScreen<PurgeScreen>();
            break;
    }
}

void DrumScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "drum")
    {
        setDrum(drum + i);
    }
    else if (focusedFieldName == "pgm")
    {
        getActiveDrumBus()->setProgram(getActiveDrumBus()->getProgram() + i);
        displayPgm();
    }
    else if (focusedFieldName == "program-change")
    {
        getActiveDrumBus()->setReceivePgmChange(i > 0);
        displayPgmChange();
    }
    else if (focusedFieldName == "midi-volume")
    {
        getActiveDrumBus()->setReceiveMidiVolume(i > 0);
        displayMidiVolume();
    }
    else if (focusedFieldName == "current-val")
    {
        getActiveDrumBus()->setLastReceivedMidiVolume(
            getActiveDrumBus()->getLastReceivedMidiVolume() + i);
        displayCurrentVal();
    }
    else if (focusedFieldName == "padtointernalsound")
    {
        setPadToIntSound(i > 0);
    }
}

void DrumScreen::displayCurrentVal()
{
    findField("current-val")
        ->setTextPadded(getActiveDrumBus()->getLastReceivedMidiVolume());
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
    auto pn = getActiveDrumBus()->getProgram();
    findField("pgm")->setText(StrUtil::padLeft(std::to_string(pn + 1), " ", 2) +
                              "-" + sampler->getProgram(pn)->getName());
}

void DrumScreen::displayPgmChange()
{
    findField("program-change")
        ->setText(getActiveDrumBus()->receivesPgmChange() ? "RECEIVE" : "IGNORE");
}

void DrumScreen::displayMidiVolume()
{
    findField("midi-volume")
        ->setText(getActiveDrumBus()->receivesMidiVolume() ? "RECEIVE" : "IGNORE");
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
