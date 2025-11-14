#include "lcdgui/ScreenIdGroups.hpp"

namespace mpc::lcdgui::screengroups
{
    bool isPadDoesNotTriggerNoteEventScreen(const ScreenId screenId)
    {
        return isSoundScreen(screenId) || screenId == ScreenId::TrMuteScreen ||
               screenId == ScreenId::NextSeqPadScreen ||
               screenId == ScreenId::PopupScreen;
    }

    bool isSoundScreen(const ScreenId screenId)
    {
        return screenId == ScreenId::TrimScreen ||
               screenId == ScreenId::LoopScreen ||
               screenId == ScreenId::ZoneScreen ||
               screenId == ScreenId::SndParamsScreen ||
               screenId == ScreenId::SoundScreen ||
               screenId == ScreenId::StartFineScreen ||
               screenId == ScreenId::EndFineScreen ||
               screenId == ScreenId::LoopToFineScreen ||
               screenId == ScreenId::LoopEndFineScreen ||
               screenId == ScreenId::ZoneStartFineScreen ||
               screenId == ScreenId::ZoneEndFineScreen ||
               screenId == ScreenId::NumberOfZonesScreen ||
               screenId == ScreenId::EditSoundScreen ||
               screenId == ScreenId::DeleteSoundScreen ||
               screenId == ScreenId::DeleteAllSoundScreen ||
               screenId == ScreenId::ConvertSoundScreen ||
               screenId == ScreenId::StereoToMonoScreen ||
               screenId == ScreenId::MonoToStereoScreen ||
               screenId == ScreenId::ResampleScreen;
    }

    bool isSongScreen(const ScreenId screenId)
    {
        return screenId == ScreenId::SongScreen ||
               screenId == ScreenId::SongWindow ||
               screenId == ScreenId::LoopSongScreen ||
               screenId == ScreenId::IgnoreTempoChangeScreen;
    }
} // namespace mpc::lcdgui::screengroups
