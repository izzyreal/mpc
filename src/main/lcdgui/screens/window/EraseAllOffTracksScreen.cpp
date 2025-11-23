#include "lcdgui/screens/window/EraseAllOffTracksScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

EraseAllOffTracksScreen::EraseAllOffTracksScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "erase-all-off-tracks", layerIndex)
{
}

void EraseAllOffTracksScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
        {
            const auto seq = sequencer.lock()->getSelectedSequence();
            int trackCounter = 0;

            for (const auto &track : seq->getTracks())
            {
                if (!track->isOn())
                {
                    seq->purgeTrack(trackCounter);
                }
                trackCounter++;
            }
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
    }
}
