#include "TransposePermanentScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/TransScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteOnEvent.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

TransposePermanentScreen::TransposePermanentScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "transpose-permanent", layerIndex)
{
}

void TransposePermanentScreen::function(const int i)
{
    ScreenComponent::function(i); // For closing this window

    if (i == 4)
    {
        const auto transScreen = mpc.screens->get<ScreenId::TransScreen>();
        const auto all = transScreen->getTr() == TransScreen::ALL_TRACKS;

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
            tracks.push_back(transScreen->getTr());
        }

        const auto seq = sequencer->getSelectedSequence();
        const auto firstTick = seq->getFirstTickOfBar(transScreen->getBar0());
        const auto lastTick = seq->getLastTickOfBar(transScreen->getBar1());

        for (const auto &trackIndex : tracks)
        {
            const auto t = seq->getTrack(trackIndex);

            for (const auto &n : t->getNoteEvents())
            {
                if (n->getTick() < firstTick || n->getTick() > lastTick)
                {
                    continue;
                }

                n->setNote(n->getNote() + transScreen->getTransposeAmount());
            }
        }
        transScreen->setTransposeAmount(0);
        openScreenById(ScreenId::SequencerScreen);
    }
}
