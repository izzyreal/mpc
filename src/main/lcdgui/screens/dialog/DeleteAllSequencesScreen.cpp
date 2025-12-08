#include "DeleteAllSequencesScreen.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
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
        mpc.getEngineHost()->postToAudioThread(utils::Task(
            [this, lockedSequencer]
            {
                lockedSequencer->deleteAllSequences();
                lockedSequencer->getStateManager()->drainQueue();
                ls.lock()->postToUiThread(utils::Task(
                    [this]
                    {
                        openScreenById(ScreenId::SequencerScreen);
                    }));
            }));
    }
}
