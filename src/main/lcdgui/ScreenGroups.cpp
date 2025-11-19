#include "lcdgui/ScreenGroups.hpp"

#include "lcdgui/AllScreens.hpp"
#include <memory>

namespace mpc::lcdgui::screengroups
{
    bool isPadDoesNotTriggerNoteEventScreen(
        const std::shared_ptr<ScreenComponent> &s)
    {
        return isSoundScreen(s) || std::dynamic_pointer_cast<TrMuteScreen>(s) ||
               std::dynamic_pointer_cast<NextSeqPadScreen>(s) ||
               std::dynamic_pointer_cast<PopupScreen>(s);
    }

    bool isStepEditorScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<StepEditorScreen>(s) ||
               std::dynamic_pointer_cast<PasteEventScreen>(s) ||
               std::dynamic_pointer_cast<InsertEventScreen>(s) ||
               std::dynamic_pointer_cast<EditMultipleScreen>(s) ||
               std::dynamic_pointer_cast<StepTcScreen>(s);
    }

    bool isNextSeqScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<SequencerScreen>(s) ||
               std::dynamic_pointer_cast<NextSeqScreen>(s) ||
               std::dynamic_pointer_cast<NextSeqPadScreen>(s) ||
               std::dynamic_pointer_cast<TrMuteScreen>(s) ||
               std::dynamic_pointer_cast<TimeDisplayScreen>(s) ||
               std::dynamic_pointer_cast<AssignScreen>(s);
    }

    bool isScreenThatIsNotAllowedToOpenMainScreen(
        const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<FileExistsScreen>(s) ||
               std::dynamic_pointer_cast<VmpcDiscardMappingChangesScreen>(s) ||
               std::dynamic_pointer_cast<VmpcResetKeyboardScreen>(s);
    }

    bool isPlayOnlyScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<SongScreen>(s) ||
               std::dynamic_pointer_cast<TrMuteScreen>(s) ||
               std::dynamic_pointer_cast<NextSeqScreen>(s) ||
               std::dynamic_pointer_cast<NextSeqPadScreen>(s) ||
               std::dynamic_pointer_cast<VmpcRecordingFinishedScreen>(s);
    }

    bool
    isCentralNoteAndPadUpdateScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<PgmAssignScreen>(s) ||
               std::dynamic_pointer_cast<PgmParamsScreen>(s) ||
               std::dynamic_pointer_cast<VelocityModulationScreen>(s) ||
               std::dynamic_pointer_cast<VeloEnvFilterScreen>(s) ||
               std::dynamic_pointer_cast<VeloPitchScreen>(s) ||
               std::dynamic_pointer_cast<MuteAssignScreen>(s) ||
               std::dynamic_pointer_cast<AssignmentViewScreen>(s) ||
               std::dynamic_pointer_cast<KeepOrRetryScreen>(s) ||
               std::dynamic_pointer_cast<LoadASoundScreen>(s);
    }

    bool isPlayAndRecordScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<SequencerScreen>(s) ||
               std::dynamic_pointer_cast<SelectDrumScreen>(s) ||
               std::dynamic_pointer_cast<SelectMixerDrumScreen>(s) ||
               std::dynamic_pointer_cast<PgmAssignScreen>(s) ||
               std::dynamic_pointer_cast<PgmParamsScreen>(s) ||
               std::dynamic_pointer_cast<DrumScreen>(s) ||
               std::dynamic_pointer_cast<PurgeScreen>(s) ||
               std::dynamic_pointer_cast<ProgramScreen>(s) ||
               std::dynamic_pointer_cast<CreateNewProgramScreen>(s) ||
               std::dynamic_pointer_cast<NameScreen>(s) ||
               std::dynamic_pointer_cast<DeleteProgramScreen>(s) ||
               std::dynamic_pointer_cast<DeleteAllProgramsScreen>(s) ||
               std::dynamic_pointer_cast<AssignmentViewScreen>(s) ||
               std::dynamic_pointer_cast<InitPadAssignScreen>(s) ||
               std::dynamic_pointer_cast<CopyNoteParametersScreen>(s) ||
               std::dynamic_pointer_cast<VelocityModulationScreen>(s) ||
               std::dynamic_pointer_cast<VeloEnvFilterScreen>(s) ||
               std::dynamic_pointer_cast<VeloPitchScreen>(s) ||
               std::dynamic_pointer_cast<MuteAssignScreen>(s) ||
               std::dynamic_pointer_cast<TransScreen>(s) ||
               std::dynamic_pointer_cast<MixerScreen>(s) ||
               std::dynamic_pointer_cast<MixerSetupScreen>(s) ||
               std::dynamic_pointer_cast<ChannelSettingsScreen>(s);
    }

    bool isPlayScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return isPlayOnlyScreen(s) || isPlayAndRecordScreen(s);
    }

    bool isSamplerScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<CreateNewProgramScreen>(s) ||
               std::dynamic_pointer_cast<AssignmentViewScreen>(s) ||
               std::dynamic_pointer_cast<AutoChromaticAssignmentScreen>(s) ||
               std::dynamic_pointer_cast<CopyNoteParametersScreen>(s) ||
               std::dynamic_pointer_cast<EditSoundScreen>(s) ||
               std::dynamic_pointer_cast<EndFineScreen>(s) ||
               std::dynamic_pointer_cast<InitPadAssignScreen>(s) ||
               std::dynamic_pointer_cast<KeepOrRetryScreen>(s) ||
               std::dynamic_pointer_cast<LoopEndFineScreen>(s) ||
               std::dynamic_pointer_cast<LoopToFineScreen>(s) ||
               std::dynamic_pointer_cast<MuteAssignScreen>(s) ||
               std::dynamic_pointer_cast<ProgramScreen>(s) ||
               std::dynamic_pointer_cast<DeleteProgramScreen>(s) ||
               std::dynamic_pointer_cast<CopyProgramScreen>(s) ||
               std::dynamic_pointer_cast<StartFineScreen>(s) ||
               std::dynamic_pointer_cast<VeloEnvFilterScreen>(s) ||
               std::dynamic_pointer_cast<VeloPitchScreen>(s) ||
               std::dynamic_pointer_cast<VelocityModulationScreen>(s) ||
               std::dynamic_pointer_cast<ZoneEndFineScreen>(s) ||
               std::dynamic_pointer_cast<ZoneStartFineScreen>(s) ||
               std::dynamic_pointer_cast<DrumScreen>(s) ||
               std::dynamic_pointer_cast<LoopScreen>(s) ||
               std::dynamic_pointer_cast<MixerScreen>(s) ||
               std::dynamic_pointer_cast<MixerSetupScreen>(s) ||
               std::dynamic_pointer_cast<ChannelSettingsScreen>(s) ||
               std::dynamic_pointer_cast<PgmAssignScreen>(s) ||
               std::dynamic_pointer_cast<PgmParamsScreen>(s) ||
               std::dynamic_pointer_cast<SelectDrumScreen>(s) ||
               std::dynamic_pointer_cast<SelectMixerDrumScreen>(s) ||
               std::dynamic_pointer_cast<TrimScreen>(s) ||
               std::dynamic_pointer_cast<ZoneScreen>(s) ||
               std::dynamic_pointer_cast<LoadASoundScreen>(s);
    }

    bool isSoundScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<TrimScreen>(s) ||
               std::dynamic_pointer_cast<LoopScreen>(s) ||
               std::dynamic_pointer_cast<ZoneScreen>(s) ||
               std::dynamic_pointer_cast<SndParamsScreen>(s) ||
               std::dynamic_pointer_cast<SoundScreen>(s) ||
               std::dynamic_pointer_cast<StartFineScreen>(s) ||
               std::dynamic_pointer_cast<EndFineScreen>(s) ||
               std::dynamic_pointer_cast<LoopToFineScreen>(s) ||
               std::dynamic_pointer_cast<LoopEndFineScreen>(s) ||
               std::dynamic_pointer_cast<ZoneStartFineScreen>(s) ||
               std::dynamic_pointer_cast<ZoneEndFineScreen>(s) ||
               std::dynamic_pointer_cast<NumberOfZonesScreen>(s) ||
               std::dynamic_pointer_cast<EditSoundScreen>(s) ||
               std::dynamic_pointer_cast<DeleteSoundScreen>(s) ||
               std::dynamic_pointer_cast<DeleteAllSoundScreen>(s) ||
               std::dynamic_pointer_cast<ConvertSoundScreen>(s) ||
               std::dynamic_pointer_cast<StereoToMonoScreen>(s) ||
               std::dynamic_pointer_cast<MonoToStereoScreen>(s) ||
               std::dynamic_pointer_cast<ResampleScreen>(s);
    }

    bool isSongScreen(const std::shared_ptr<ScreenComponent> &s)
    {
        return std::dynamic_pointer_cast<SongScreen>(s) ||
               std::dynamic_pointer_cast<SongWindow>(s) ||
               std::dynamic_pointer_cast<LoopSongScreen>(s) ||
               std::dynamic_pointer_cast<IgnoreTempoChangeScreen>(s);
    }
} // namespace mpc::lcdgui::screengroups
