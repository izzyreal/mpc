#include "DeleteTrackScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "StrUtil.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteTrackScreen::DeleteTrackScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-track", layerIndex)
{
}

void DeleteTrackScreen::open()
{
    setTr(sequencer.lock()->getSelectedTrackIndex());
    displayTr();
}

void DeleteTrackScreen::turnWheel(const int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "tr")
    {
        setTr(tr + i);
    }
}

void DeleteTrackScreen::function(const int i)
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
            sequencer.lock()->copySelectedSequenceToUndoSequence();
            const auto s = sequencer.lock()->getSelectedSequence();
            s->deleteTrack(TrackIndex(tr));
            openScreenById(ScreenId::SequencerScreen);
        }
        default:;
    }
}

void DeleteTrackScreen::setTr(const int i)
{
    tr = std::clamp(i, 0, 63);
    displayTr();
}

void DeleteTrackScreen::displayTr() const
{
    const auto trackName =
        sequencer.lock()->getSelectedSequence()->getTrack(tr)->getName();
    findField("tr")->setText(StrUtil::padLeft(std::to_string(tr + 1), "0", 2) +
                             "-" + trackName);
}
