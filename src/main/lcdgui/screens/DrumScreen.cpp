#include "DrumScreen.hpp"

#include "Mpc.hpp"
#include "SelectDrumScreen.hpp"
#include "StrUtil.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens;

DrumScreen::DrumScreen(Mpc &mpc, const int layerIndex)
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
            openScreenById(ScreenId::PgmAssignScreen);
            break;
        case 1:
            openScreenById(ScreenId::PgmParamsScreen);
            break;
        case 2:
        {
            const auto selectDrumScreen =
                mpc.screens->get<ScreenId::SelectDrumScreen>();
            selectDrumScreen->redirectScreen = "drum";
            openScreenById(ScreenId::SelectDrumScreen);
            break;
        }
        case 3:
            openScreenById(ScreenId::PurgeScreen);
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

void DrumScreen::displayCurrentVal() const
{
    findField("current-val")
        ->setTextPadded(getActiveDrumBus()->getLastReceivedMidiVolume());
}

void DrumScreen::displayDrum() const
{
    findField("drum")->setText(std::to_string(drum + 1));
}

void DrumScreen::displayPadToInternalSound() const
{
    findField("padtointernalsound")->setText(padToInternalSound ? "ON" : "OFF");
}

void DrumScreen::displayPgm() const
{
    const auto pn = getActiveDrumBus()->getProgram();
    findField("pgm")->setText(StrUtil::padLeft(std::to_string(pn + 1), " ", 2) +
                              "-" + sampler->getProgram(pn)->getName());
}

void DrumScreen::displayPgmChange() const
{
    findField("program-change")
        ->setText(getActiveDrumBus()->receivesPgmChange() ? "RECEIVE"
                                                          : "IGNORE");
}

void DrumScreen::displayMidiVolume() const
{
    findField("midi-volume")
        ->setText(getActiveDrumBus()->receivesMidiVolume() ? "RECEIVE"
                                                           : "IGNORE");
}

bool DrumScreen::isPadToIntSound() const
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

unsigned char DrumScreen::getDrum() const
{
    return drum;
}
