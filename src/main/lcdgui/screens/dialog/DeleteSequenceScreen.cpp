#include "DeleteSequenceScreen.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteSequenceScreen::DeleteSequenceScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-sequence", layerIndex)
{
}

void DeleteSequenceScreen::open()
{
    displaySequenceNumberName();
}

void DeleteSequenceScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sq")
    {
        sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() +
                                          i);
        displaySequenceNumberName();
    }
}

void DeleteSequenceScreen::function(int i)
{

    switch (i)
    {
        case 2:
            mpc.getLayeredScreen()->openScreen<DeleteAllSequencesScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
            break;
        case 4:
            sequencer->move(0);
            sequencer->purgeSequence(sequencer->getActiveSequenceIndex());
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}

void DeleteSequenceScreen::displaySequenceNumberName()
{
    auto sequenceName = sequencer->getActiveSequence()->getName();
    findField("sq")->setText(
        StrUtil::padLeft(
            std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) +
        "-" + sequenceName);
}
