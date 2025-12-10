#include "CopySequenceScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopySequenceScreen::CopySequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-sequence", layerIndex)
{
    addReactiveBinding({[&]
                        {
                            return sequencer.lock()->getSelectedSequenceIndex();
                        },
                        [&](auto)
                        {
                            displaySq0();
                        }});
}

void CopySequenceScreen::open()
{
    sq1 = sequencer.lock()->getFirstUsedSeqUp(MinSequenceIndex, true);

    displaySq0();
    displaySq1();
}

void CopySequenceScreen::function(const int i)
{
    const auto lockedSequencer = sequencer.lock();
    switch (i)
    {
        case 2:
            lockedSequencer->copySequenceParameters(
                lockedSequencer->getSelectedSequence(), sq1);
            openScreenById(ScreenId::SequencerScreen);
            break;
        case 3:
            openScreenById(ScreenId::SequenceScreen);
            break;
        case 4:
        {
            lockedSequencer->copySequence(
                lockedSequencer->getSelectedSequenceIndex(), sq1);
            constexpr bool shouldSetPositionTo0 = true;
            lockedSequencer->setSelectedSequenceIndex(sq1,
                                                      shouldSetPositionTo0);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
        default:;
    }
}

void CopySequenceScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("0") != std::string::npos)
    {
        setSq0(sequencer.lock()->getSelectedSequenceIndex() + increment);
    }
    else if (focusedFieldName.find("1") != std::string::npos)
    {
        setSq1(sq1 + increment);
    }
}

void CopySequenceScreen::setSq0(const SequenceIndex sequenceIndex) const
{
    constexpr bool shouldSetPositionTo0 = true;
    sequencer.lock()->setSelectedSequenceIndex(sequenceIndex,
                                               shouldSetPositionTo0);
}

void CopySequenceScreen::setSq1(const SequenceIndex sequenceIndex)
{
    sq1 = std::clamp(sequenceIndex, MinSequenceIndex, MaxSequenceIndex);
    displaySq1();
}

void CopySequenceScreen::displaySq0() const
{
    const auto lockedSequencer = sequencer.lock();
    const auto sq0Name = lockedSequencer->getSelectedSequence()->getName();
    findField("sq0")->setText(
        StrUtil::padLeft(
            std::to_string(lockedSequencer->getSelectedSequenceIndex() + 1),
            "0", 2) +
        "-" + sq0Name);
}

void CopySequenceScreen::displaySq1() const
{
    const auto sq1Name = sequencer.lock()->getSequence(sq1)->getName();
    findField("sq1")->setText(
        StrUtil::padLeft(std::to_string(sq1 + 1), "0", 2) + "-" + sq1Name);
}
