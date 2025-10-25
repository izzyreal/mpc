#include "CopySequenceScreen.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

CopySequenceScreen::CopySequenceScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-sequence", layerIndex)
{
}

void CopySequenceScreen::open()
{
    sq0 = sequencer.lock()->getActiveSequenceIndex();
    sq1 = sequencer.lock()->getFirstUsedSeqUp(0, true);

    displaySq0();
    displaySq1();
}

void CopySequenceScreen::function(int i)
{

    switch (i)
    {
        case 2:
            sequencer.lock()->copySequenceParameters(sq0, sq1);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
        case 3:
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
            break;
        case 4:
            sequencer.lock()->copySequence(sq0, sq1);
            sequencer.lock()->setActiveSequenceIndex(sq1);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
    }
}

void CopySequenceScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    if (focusedFieldName.find("0") != std::string::npos)
    {
        setSq0(sq0 + i);
    }
    else if (focusedFieldName.find("1") != std::string::npos)
    {
        setSq1(sq1 + i);
    }
}

void CopySequenceScreen::setSq0(int i)
{
    if (i < 0)
    {
        i = 0;
    }
    if (i > 98)
    {
        i = 98;
    }

    if (sq0 == i)
    {
        return;
    }

    sq0 = i;
    displaySq0();
}

void CopySequenceScreen::setSq1(int i)
{
    if (i < 0)
    {
        i = 0;
    }
    if (i > 98)
    {
        i = 98;
    }

    if (sq1 == i)
    {
        return;
    }

    sq1 = i;
    displaySq1();
}

void CopySequenceScreen::displaySq0()
{
    auto sq0Name = sequencer.lock()->getSequence(sq0)->getName();
    findField("sq0")->setText(StrUtil::padLeft(std::to_string(sq0 + 1), "0", 2) + "-" + sq0Name);
}

void CopySequenceScreen::displaySq1()
{
    auto sq1Name = sequencer.lock()->getSequence(sq1)->getName();
    findField("sq1")->setText(StrUtil::padLeft(std::to_string(sq1 + 1), "0", 2) + "-" + sq1Name);
}
