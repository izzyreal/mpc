#include "DeleteSequenceScreen.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"

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
        sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() +
                                          i);
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
            sequencer->getTransport()->setPosition(0);
            sequencer->purgeSequence(sequencer->getActiveSequenceIndex());
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}

void DeleteSequenceScreen::displaySequenceNumberName() const
{
    const auto sequenceName = sequencer->getActiveSequence()->getName();
    findField("sq")->setText(
        StrUtil::padLeft(
            std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) +
        "-" + sequenceName);
}
