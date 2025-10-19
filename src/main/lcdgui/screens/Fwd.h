#pragma once

namespace mpc::lcdgui::screens {
    class SequencerScreen;
    class AssignScreen;
    class BarsScreen;
    class EventsScreen;
    class NextSeqPadScreen;
    class NextSeqScreen;
    class SongScreen;
    class StepEditorScreen;
    class TrMoveScreen;
    class TrMuteScreen;
    class UserScreen;
    class OthersScreen;
    class DrumScreen;
    class LoopScreen;
    class PgmAssignScreen;
    class PgmParamsScreen;
    class PurgeScreen;
    class SampleScreen;
    class SelectDrumScreen;
    class SndParamsScreen;
    class TrimScreen;
    class ZoneScreen;
    class MixerScreen;
    class MixerSetupScreen;
    class FxEditScreen;
    class SelectMixerDrumScreen;
    class LoadScreen;
    class FormatScreen;
    class SetupScreen;
    class SaveScreen;
    class VerScreen;
    class InitScreen;
    class SecondSeqScreen;
    class TransScreen;
    class PunchScreen;
    class SyncScreen;
    class MidiSwScreen;

    class VmpcDisksScreen;
    class VmpcSettingsScreen;
    class VmpcKeyboardScreen;
    class VmpcMidiScreen;
    class VmpcAutoSaveScreen;
}

namespace mpc::lcdgui::screens::window {
    class NumberOfZonesScreen;
    class InitPadAssignScreen;
    class SequenceScreen;
    class TempoChangeScreen;
    class CountMetronomeScreen;
    class EditMultipleScreen;
    class TransmitProgramChangesScreen;
    class TimingCorrectScreen;
    class TimeDisplayScreen;
    class PasteEventScreen;
    class MultiRecordingSetupScreen;
    class MidiOutputScreen;
    class MidiInputScreen;
    class LoopBarsScreen;
    class InsertEventScreen;
    class EraseAllOffTracksScreen;
    class ChangeTsigScreen;
    class EditVelocityScreen;
    class EraseScreen;
    class ChangeBarsScreen;
    class ChangeBars2Screen;
    class TrackScreen;
    class Assign16LevelsScreen;
    class StepTcScreen;
    class SoundScreen;
    class StartFineScreen;
    class EndFineScreen;
    class LoopToFineScreen;
    class LoopEndFineScreen;
    class ZoneStartFineScreen;
    class ZoneEndFineScreen;
    class ChannelSettingsScreen;
    class EditSoundScreen;
    class AssignmentViewScreen;
    class AutoChromaticAssignmentScreen;
    class CopyNoteParametersScreen;
    class KeepOrRetryScreen;
    class MuteAssignScreen;
    class ProgramScreen;
    class VelocityModulationScreen;
    class VeloEnvFilterScreen;
    class VeloPitchScreen;
    class DirectoryScreen;
    class LoadAProgramScreen;
    class LoadASequenceScreen;
    class LoadASequenceFromAllScreen;
    class LoadASoundScreen;
    class LoadApsFileScreen;
    class Mpc2000XlAllFileScreen;
    class SaveAProgramScreen;
    class SaveASequenceScreen;
    class SaveASoundScreen;
    class SaveAllFileScreen;
    class SaveApsFileScreen;
    class CantFindFileScreen;
    class NameScreen;
    class TransposePermanentScreen;
    class SoundMemoryScreen;
    class SongWindow;
    class IgnoreTempoChangeScreen;
    class LoopSongScreen;
    class StepEditOptionsScreen;
    class ConvertSongToSeqScreen;
    class LocateScreen;

    class VmpcWarningSettingsIgnoredScreen;
    class VmpcKnownControllerDetectedScreen;
    class VmpcContinuePreviousSessionScreen;
    class VmpcConvertAndLoadWavScreen;
    class VmpcDiscardMappingChangesScreen;
    class VmpcRecordingFinishedScreen;
    class VmpcResetKeyboardScreen;
    class VmpcDirectToDiskRecorderScreen;
    class VmpcMidiPresetsScreen;
}

namespace mpc::lcdgui::screens::dialog {
    class MetronomeSoundScreen;
    class MidiInputMonitorScreen;
    class MidiOutputMonitorScreen;
    class CopySequenceScreen;
    class CopyTrackScreen;
    class DeleteTrackScreen;
    class DeleteSequenceScreen;
    class DeleteAllSequencesScreen;
    class DeleteAllTracksScreen;
    class ConvertSoundScreen;
    class CopySoundScreen;
    class DeleteSoundScreen;
    class MonoToStereoScreen;
    class ResampleScreen;
    class StereoToMonoScreen;
    class CreateNewProgramScreen;
    class CopyProgramScreen;
    class DeleteAllProgramsScreen;
    class DeleteAllSoundScreen;
    class DeleteProgramScreen;
    class DeleteFileScreen;
    class DeleteFolderScreen;
    class FileExistsScreen;
    class DeleteAllSongScreen;
    class DeleteSongScreen;
    class CopySongScreen;

    class VmpcRecordJamScreen;
    class VmpcFileInUseScreen;
}

namespace mpc::lcdgui::screens::dialog2 {
    class DeleteAllFilesScreen;
    class PopupScreen;
}

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

