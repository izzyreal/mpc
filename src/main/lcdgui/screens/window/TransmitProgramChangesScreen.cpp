#include "TransmitProgramChangesScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

TransmitProgramChangesScreen::TransmitProgramChangesScreen(Mpc &mpc,
                                                           const int layerIndex)
    : ScreenComponent(mpc, "transmit-program-changes", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()
                                ->getSelectedTrack()
                                ->isTransmitProgramChangesEnabled();
                        },
                        [&](auto)
                        {
                            displayTransmitProgramChangesInThisTrack();
                        }});
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
        sequencer.lock()->getSelectedTrack()->setTransmitProgramChangesEnabled(
            i > 0);
    }
}

void TransmitProgramChangesScreen::displayTransmitProgramChangesInThisTrack()
    const
{
    findField("inthistrack")
        ->setText(sequencer.lock()
                          ->getSelectedTrack()
                          ->isTransmitProgramChangesEnabled()
                      ? "YES"
                      : "NO");
}
