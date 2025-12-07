#include "lcdgui/screens/window/EraseAllOffTracksScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

EraseAllOffTracksScreen::EraseAllOffTracksScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "erase-all-off-tracks", layerIndex)
{
}

void EraseAllOffTracksScreen::function(const int i)
{
    ScreenComponent::function(i);

    if (i != 4)
    {
        return;
    }

    const auto lockedSequencer = sequencer.lock();
    const auto seq = lockedSequencer->getSelectedSequence();
    int trackCounter = 0;

    for (const auto &track : seq->getTracks())
    {
        if (!track->isOn())
        {
            track->removeEvents();
            lockedSequencer->getStateManager()->enqueue(sequencer::SetTrackUsed{
                seq->getSequenceIndex(), track->getIndex(), false});
        }

        trackCounter++;
    }
    openScreenById(ScreenId::SequencerScreen);
}
