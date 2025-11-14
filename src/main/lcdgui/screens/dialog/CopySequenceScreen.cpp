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
    sq0 = sequencer->getSelectedSequenceIndex();
    sq1 = sequencer->getFirstUsedSeqUp(MinSequenceIndex, true);

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
            sequencer->setSelectedSequenceIndex(sq1, true);
            openScreenById(ScreenId::SequencerScreen);
            break;
        default:;
    }
}

void CopySequenceScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("0") != std::string::npos)
    {
        setSq0(sq0 + increment);
    }
    else if (focusedFieldName.find("1") != std::string::npos)
    {
        setSq1(sq1 + increment);
    }
}

void CopySequenceScreen::setSq0(const SequenceIndex sequenceIndex)
{
    sq0 = std::clamp(sequenceIndex, MinSequenceIndex, MaxSequenceIndex);
    displaySq0();
}

void CopySequenceScreen::setSq1(const SequenceIndex sequenceIndex)
{
    sq1 = std::clamp(sequenceIndex, MinSequenceIndex, MaxSequenceIndex);
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
