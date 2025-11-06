#include "CopySequenceScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopySequenceScreen::CopySequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-sequence", layerIndex)
{
}

void CopySequenceScreen::open()
{
    sq0 = sequencer->getActiveSequenceIndex();
    sq1 = sequencer->getFirstUsedSeqUp(0, true);

    displaySq0();
    displaySq1();
}

void CopySequenceScreen::function(const int i)
{
    switch (i)
    {
        case 2:
            sequencer->copySequenceParameters(sq0, sq1);
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 3:
            openScreenById(ScreenId::SequenceScreen);
            break;
        case 4:
            sequencer->copySequence(sq0, sq1);
            sequencer->setActiveSequenceIndex(sq1);
            openScreenById(ScreenId::SequencerScreen);
            break;
    }
}

void CopySequenceScreen::turnWheel(const int i)
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

void CopySequenceScreen::displaySq0() const
{
    const auto sq0Name = sequencer->getSequence(sq0)->getName();
    findField("sq0")->setText(
        StrUtil::padLeft(std::to_string(sq0 + 1), "0", 2) + "-" + sq0Name);
}

void CopySequenceScreen::displaySq1() const
{
    const auto sq1Name = sequencer->getSequence(sq1)->getName();
    findField("sq1")->setText(
        StrUtil::padLeft(std::to_string(sq1 + 1), "0", 2) + "-" + sq1Name);
}
