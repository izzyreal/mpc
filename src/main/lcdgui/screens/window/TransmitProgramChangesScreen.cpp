#include "TransmitProgramChangesScreen.hpp"

using namespace mpc::lcdgui::screens::window;

TransmitProgramChangesScreen::TransmitProgramChangesScreen(Mpc &mpc,
                                                           const int layerIndex)
    : ScreenComponent(mpc, "transmit-program-changes", layerIndex)
{
}

void TransmitProgramChangesScreen::open()
{
    displayTransmitProgramChangesInThisTrack();
}

void TransmitProgramChangesScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "inthistrack")
    {
        transmitProgramChangesInThisTrack = i > 0;
        displayTransmitProgramChangesInThisTrack();
    }
}

void TransmitProgramChangesScreen::displayTransmitProgramChangesInThisTrack()
    const
{
    findField("inthistrack")
        ->setText(transmitProgramChangesInThisTrack ? "YES" : "NO");
}

bool TransmitProgramChangesScreen::isTransmitProgramChangesInThisTrackEnabled()
    const
{
    return transmitProgramChangesInThisTrack;
}
