#include "CopyTrackScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

CopyTrackScreen::CopyTrackScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-track", layerIndex)
{
}

void CopyTrackScreen::open()
{
    displayTr0();
    displayTr1();
}

void CopyTrackScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::TrackScreen);
            break;
        case 4:
        {
            const auto seqIndex = sequencer.lock()->getSelectedSequenceIndex();
            sequencer.lock()->copyTrack(tr0, tr1, seqIndex, seqIndex);
            openScreenById(ScreenId::SequencerScreen);
            break;
        }
    }
}

void CopyTrackScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("0") != std::string::npos)
    {
        setTr0(tr0 + i);
    }
    else if (focusedFieldName.find("1") != std::string::npos)
    {
        setTr1(tr1 + i);
    }
}

void CopyTrackScreen::setTr0(const int i)
{
    tr0 = std::clamp(i, 0, 63);
    displayTr0();
}

void CopyTrackScreen::setTr1(const int i)
{
    tr1 = std::clamp(i, 0, 63);
    displayTr1();
}

void CopyTrackScreen::displayTr0() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    const auto tr0Name = seq->getTrack(tr0)->getName();
    findField("tr0")->setText(
        StrUtil::padLeft(std::to_string(tr0 + 1), "0", 2) + "-" + tr0Name);
}

void CopyTrackScreen::displayTr1() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    const auto tr1Name = seq->getTrack(tr1)->getName();
    findField("tr1")->setText(
        StrUtil::padLeft(std::to_string(tr1 + 1), "0", 2) + "-" + tr1Name);
}
