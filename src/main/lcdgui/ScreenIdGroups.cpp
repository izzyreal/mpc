#include "lcdgui/ScreenIdGroups.hpp"

namespace mpc::lcdgui::screengroups
{
    bool isPadDoesNotTriggerNoteEventScreen(const ScreenId screenId)
    {
        return isSoundScreen(screenId) || screenId == ScreenId::TrMuteScreen ||
               screenId == ScreenId::NextSeqPadScreen ||
               screenId == ScreenId::PopupScreen;
    }

    bool isSamplerScreen(const ScreenId screenId)
    {
        return screenId == ScreenId::CreateNewProgramScreen ||
               screenId == ScreenId::AssignmentViewScreen ||
               screenId == ScreenId::AutoChromaticAssignmentScreen ||
               screenId == ScreenId::CopyNoteParametersScreen ||
               screenId == ScreenId::EditSoundScreen ||
               screenId == ScreenId::EndFineScreen ||
               screenId == ScreenId::InitPadAssignScreen ||
               screenId == ScreenId::KeepOrRetryScreen ||
               screenId == ScreenId::LoopEndFineScreen ||
               screenId == ScreenId::LoopToFineScreen ||
               screenId == ScreenId::MuteAssignScreen ||
               screenId == ScreenId::ProgramScreen ||
               screenId == ScreenId::DeleteProgramScreen ||
               screenId == ScreenId::CopyProgramScreen ||
               screenId == ScreenId::StartFineScreen ||
               screenId == ScreenId::VeloEnvFilterScreen ||
               screenId == ScreenId::VeloPitchScreen ||
               screenId == ScreenId::VelocityModulationScreen ||
               screenId == ScreenId::ZoneEndFineScreen ||
               screenId == ScreenId::ZoneStartFineScreen ||
               screenId == ScreenId::DrumScreen ||
               screenId == ScreenId::LoopScreen ||
               screenId == ScreenId::MixerScreen ||
               screenId == ScreenId::MixerSetupScreen ||
               screenId == ScreenId::ChannelSettingsScreen ||
               screenId == ScreenId::PgmAssignScreen ||
               screenId == ScreenId::PgmParamsScreen ||
               screenId == ScreenId::SelectDrumScreen ||
               screenId == ScreenId::SelectMixerDrumScreen ||
               screenId == ScreenId::TrimScreen ||
               screenId == ScreenId::ZoneScreen ||
               screenId == ScreenId::LoadASoundScreen;
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
