#include "DeleteAllSequencesScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSequencesScreen::DeleteAllSequencesScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "delete-all-sequences", layerIndex)
{
}

void DeleteAllSequencesScreen::function(const int i)
{
    if (i == 3)
    {
        openScreenById(ScreenId::DeleteSequenceScreen);
    }
    else if (i == 4)
    {
        const auto lockedSequencer = sequencer.lock();
        lockedSequencer->getTransport()->setPosition(0);
        lockedSequencer->deleteAllSequences();
        lockedSequencer->getStateManager()->enqueueCallback(
            utils::SimpleAction(
                [layeredScreen = ls]
            {
                if (const auto lockedLayeredScreen = layeredScreen.lock())
                {
                    lockedLayeredScreen->postToUiThread(utils::Task(
                        [layeredScreen]
                    {
                        if (const auto lockedLayeredScreen =
                                layeredScreen.lock())
                        {
                            lockedLayeredScreen->openScreenById(
                                ScreenId::SequencerScreen);
                        }
                    }));
                }
            }));
    }
}
