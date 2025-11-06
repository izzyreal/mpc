#include "DeleteTrackScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteTrackScreen::DeleteTrackScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-track", layerIndex)
{
}

void DeleteTrackScreen::open()
{
    setTr(sequencer->getActiveTrackIndex());
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
            openScreenById(ScreenId::DeleteAllTracksScreen);
            break;
        case 3:
            openScreenById(ScreenId::TrackScreen);
            break;
        case 4:
        {
            const auto s = sequencer->getActiveSequence();
            s->purgeTrack(tr);
            openScreenById(ScreenId::SequencerScreen);
        }
    }
}

void DeleteTrackScreen::setTr(int i)
{
    tr = std::clamp(i, 0, 63);
    displayTr();
}

void DeleteTrackScreen::displayTr()
{
    const auto trackName =
        sequencer->getActiveSequence()->getTrack(tr)->getName();
    findField("tr")->setText(StrUtil::padLeft(std::to_string(tr + 1), "0", 2) +
                             "-" + trackName);
}
