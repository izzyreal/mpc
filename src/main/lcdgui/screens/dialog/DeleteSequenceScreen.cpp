#include "DeleteSequenceScreen.hpp"

#include "Mpc.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteSequenceScreen::DeleteSequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-sequence", layerIndex)
{
}

void DeleteSequenceScreen::open()
{
    displaySequenceNumberName();
}

void DeleteSequenceScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sq")
    {
        sequencer.lock()->setSelectedSequenceIndex(
            sequencer.lock()->getSelectedSequenceIndex() + i, true);
        displaySequenceNumberName();
    }
}

void DeleteSequenceScreen::function(const int i)
{
    switch (i)
    {
        case 2:
            openScreenById(ScreenId::DeleteAllSequencesScreen);
            break;
        case 3:
            openScreenById(ScreenId::SequenceScreen);
            break;
        case 4:
        {
            const auto lockedSequencer = sequencer.lock();
            lockedSequencer->getTransport()->setPosition(0);
            lockedSequencer->deleteSequence(
                lockedSequencer->getSelectedSequenceIndex());
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
            break;
        }
        default:;
    }
}

void DeleteSequenceScreen::displaySequenceNumberName() const
{
    const auto sequenceName =
        sequencer.lock()->getSelectedSequence()->getName();
    findField("sq")->setText(
        StrUtil::padLeft(
            std::to_string(sequencer.lock()->getSelectedSequenceIndex() + 1),
            "0", 2) +
        "-" + sequenceName);
}
