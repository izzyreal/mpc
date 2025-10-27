#include "CopyTrackScreen.hpp"

#include "sequencer/Track.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

CopyTrackScreen::CopyTrackScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "copy-track", layerIndex)
{
}

void CopyTrackScreen::open()
{
    displayTr0();
    displayTr1();
}

void CopyTrackScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->openScreen<TrackScreen>();
            break;
        case 4:
        {
            auto seqIndex = sequencer->getActiveSequenceIndex();
            sequencer->copyTrack(tr0, tr1, seqIndex, seqIndex);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
        }
    }
}

void CopyTrackScreen::turnWheel(int i)
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

void CopyTrackScreen::setTr0(int i)
{
    if (i < 0 || i > 63)
    {
        return;
    }

    tr0 = i;
    displayTr0();
}

void CopyTrackScreen::setTr1(int i)
{
    if (i < 0 || i > 63)
    {
        return;
    }

    tr1 = i;
    displayTr1();
}

void CopyTrackScreen::displayTr0()
{
    auto seq = sequencer->getActiveSequence();
    auto tr0Name = seq->getTrack(tr0)->getName();
    findField("tr0")->setText(StrUtil::padLeft(std::to_string(tr0 + 1), "0", 2) + "-" + tr0Name);
}

void CopyTrackScreen::displayTr1()
{
    auto seq = sequencer->getActiveSequence();
    auto tr1Name = seq->getTrack(tr1)->getName();
    findField("tr1")->setText(StrUtil::padLeft(std::to_string(tr1 + 1), "0", 2) + "-" + tr1Name);
}
