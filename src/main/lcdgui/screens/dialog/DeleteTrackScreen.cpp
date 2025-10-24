#include "DeleteTrackScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteTrackScreen::DeleteTrackScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-track", layerIndex)
{
}

void DeleteTrackScreen::open()
{
    setTr(sequencer.lock()->getActiveTrackIndex());
    displayTr();
}

void DeleteTrackScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        setTr(tr + i);
    }
}

void DeleteTrackScreen::function(int i)
{

    switch (i)
    {
    case 2:
        mpc.getLayeredScreen()->openScreen<DeleteAllTracksScreen>();
        break;
    case 3:
        mpc.getLayeredScreen()->openScreen<TrackScreen>();
        break;
    case 4:
    {
        auto s = sequencer.lock()->getActiveSequence();
        s->purgeTrack(tr);
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
    }
    }
}

void DeleteTrackScreen::setTr(int i)
{
    if (i < 0 || i > 63)
    {
        return;
    }

    tr = i;
    displayTr();
}

void DeleteTrackScreen::displayTr()
{
    auto trackName = sequencer.lock()->getActiveSequence()->getTrack(tr)->getName();
    findField("tr")->setText(StrUtil::padLeft(std::to_string(tr + 1), "0", 2) + "-" + trackName);
}
