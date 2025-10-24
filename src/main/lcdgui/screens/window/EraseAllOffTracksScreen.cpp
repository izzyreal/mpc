#include "lcdgui/screens/window/EraseAllOffTracksScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

EraseAllOffTracksScreen::EraseAllOffTracksScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "erase-all-off-tracks", layerIndex)
{
}

void EraseAllOffTracksScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
    case 4:
    {
        auto seq = sequencer.lock()->getActiveSequence();
        int trackCounter = 0;

        for (auto &track : seq->getTracks())
        {
            if (!track->isOn())
            {
                seq->purgeTrack(trackCounter);
            }
            trackCounter++;
        }
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        break;
    }
    }
}
