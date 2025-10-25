#include "TransposePermanentScreen.hpp"

#include "lcdgui/screens/TransScreen.hpp"

#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

TransposePermanentScreen::TransposePermanentScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "transpose-permanent", layerIndex)
{
}

void TransposePermanentScreen::function(int i)
{
    ScreenComponent::function(i); // For closing this window

    switch (i)
    {
        case 4:
            auto transScreen = mpc.screens->get<TransScreen>();
            auto all = transScreen->tr == -1;

            std::vector<int> tracks;

            if (all)
            {
                for (int trackIndex = 0; trackIndex < 64; trackIndex++)
                {
                    tracks.push_back(trackIndex);
                }
            }
            else
            {
                tracks.push_back(transScreen->tr);
            }

            auto seq = sequencer.lock()->getActiveSequence();
            auto firstTick = seq->getFirstTickOfBar(transScreen->bar0);
            auto lastTick = seq->getLastTickOfBar(transScreen->bar1);

            for (auto &trackIndex : tracks)
            {
                auto t = seq->getTrack(trackIndex);

                for (auto &n : t->getNoteEvents())
                {
                    if (n->getTick() < firstTick || n->getTick() > lastTick)
                    {
                        continue;
                    }

                    n->setNote(n->getNote() + transScreen->transposeAmount);
                }
            }
            transScreen->setTransposeAmount(0);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}
