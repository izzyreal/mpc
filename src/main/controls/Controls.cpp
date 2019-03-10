#include "Controls.hpp"

#include <Mpc.hpp>

#include <controls/AbstractControls.hpp>

#include <controls/GlobalReleaseControls.hpp>

#include <controls/sequencer/SequencerControls.hpp>
#include <controls/sequencer/AssignControls.hpp>
#include <controls/sequencer/BarCopyControls.hpp>
#include <controls/sequencer/EditSequenceControls.hpp>
#include <controls/sequencer/NextSeqControls.hpp>
#include <controls/sequencer/NextSeqPadControls.hpp>
#include <controls/sequencer/SongControls.hpp>
#include <controls/sequencer/StepEditorControls.hpp>
#include <controls/sequencer/TrMoveControls.hpp>
#include <controls/sequencer/TrMuteControls.hpp>
#include <controls/sequencer/UserControls.hpp>

#include <controls/sequencer/window/EraseAllOffTracksControls.hpp>
#include <controls/sequencer/window/Assign16LevelsControls.hpp>
#include <controls/sequencer/window/ChangeTsigControls.hpp>
#include <controls/sequencer/window/CountMetronomeControls.hpp>
#include <controls/sequencer/window/EditMultipleControls.hpp>
#include <controls/sequencer/window/EditVelocityControls.hpp>
#include <controls/sequencer/window/EraseControls.hpp>
#include <controls/sequencer/window/InsertEventControls.hpp>
#include <controls/sequencer/window/LoopBarsControls.hpp>
#include <controls/sequencer/window/MidiInputControls.hpp>
#include <controls/sequencer/window/MidiMonitorControls.hpp>
#include <controls/sequencer/window/MidiOutputControls.hpp>
#include <controls/sequencer/window/MultiRecordingSetupControls.hpp>
#include <controls/sequencer/window/PasteEventControls.hpp>
#include <controls/sequencer/window/SequenceControls.hpp>
#include <controls/sequencer/window/TempoChangeControls.hpp>
#include <controls/sequencer/window/TimeDisplayControls.hpp>
#include <controls/sequencer/window/TimingCorrectControls.hpp>
#include <controls/sequencer/window/TrackControls.hpp>
#include <controls/sequencer/window/TransmitProgramChangesControls.hpp>
#include <controls/sequencer/window/ChangeBars2Controls.hpp>
#include <controls/sequencer/window/ChangeBarsControls.hpp>

#include <controls/sequencer/dialog/CopySequenceControls.hpp>
#include <controls/sequencer/dialog/CopyTrackControls.hpp>
#include <controls/sequencer/dialog/DeleteAllSequencesControls.hpp>
#include <controls/sequencer/dialog/DeleteAllTracksControls.hpp>
#include <controls/sequencer/dialog/DeleteSequenceControls.hpp>
#include <controls/sequencer/dialog/DeleteTrackControls.hpp>
#include <controls/sequencer/dialog/MetronomeSoundControls.hpp>

#include <controls/sampler/DrumControls.hpp>
#include <controls/sampler/InitPadAssignControls.hpp>
#include <controls/sampler/LoopControls.hpp>
#include <controls/sampler/PgmAssignControls.hpp>
#include <controls/sampler/PgmParamsControls.hpp>
#include <controls/sampler/PurgeControls.hpp>
#include <controls/sampler/SampleControls.hpp>
#include <controls/sampler/SelectDrumControls.hpp>
#include <controls/sampler/SndParamsControls.hpp>
#include <controls/sampler/TrimControls.hpp>
#include <controls/sampler/ZoneControls.hpp>
#include <controls/sampler/dialog/ConvertSoundControls.hpp>
#include <controls/sampler/dialog/CopyProgramControls.hpp>
#include <controls/sampler/dialog/CopySoundControls.hpp>
#include <controls/sampler/dialog/CreateNewProgramControls.hpp>
#include <controls/sampler/dialog/DeleteAllProgramsControls.hpp>
#include <controls/sampler/dialog/DeleteAllSoundControls.hpp>
#include <controls/sampler/dialog/DeleteProgramControls.hpp>
#include <controls/sampler/dialog/DeleteSoundControls.hpp>
#include <controls/sampler/dialog/MonoToStereoControls.hpp>
#include <controls/sampler/dialog/ResampleControls.hpp>
#include <controls/sampler/dialog/StereoToMonoControls.hpp>
#include <controls/sampler/window/AssignmentViewControls.hpp>
#include <controls/sampler/window/AutoChromaticAssignmentControls.hpp>
#include <controls/sampler/window/CopyNoteParametersControls.hpp>
#include <controls/sampler/window/EditSoundControls.hpp>
#include <controls/sampler/window/KeepOrRetryControls.hpp>
#include <controls/sampler/window/MuteAssignControls.hpp>
#include <controls/sampler/window/NumberOfZonesControls.hpp>
#include <controls/sampler/window/ProgramControls.hpp>
#include <controls/sampler/window/SoundControls.hpp>
#include <controls/sampler/window/VeloEnvFilterControls.hpp>
#include <controls/sampler/window/VeloPitchControls.hpp>
#include <controls/sampler/window/VelocityModulationControls.hpp>
#include <controls/sampler/window/ZoomControls.hpp>

#include <controls/disk/FormatControls.hpp>
#include <controls/disk/LoadControls.hpp>
#include <controls/disk/SaveControls.hpp>
#include <controls/disk/SetupControls.hpp>

#include <controls/disk/window/DirectoryControls.hpp>
#include <controls/disk/window/LoadAProgramControls.hpp>
#include <controls/disk/window/LoadASequenceControls.hpp>
#include <controls/disk/window/LoadASequenceFromAllControls.hpp>
#include <controls/disk/window/LoadASoundControls.hpp>
#include <controls/disk/window/LoadApsFileControls.hpp>
#include <controls/disk/window/MPC2000XLAllFileControls.hpp>
#include <controls/disk/window/SaveAProgramControls.hpp>
#include <controls/disk/window/SaveASequenceControls.hpp>
#include <controls/disk/window/SaveASoundControls.hpp>
#include <controls/disk/window/SaveAllFileControls.hpp>
#include <controls/disk/window/SaveApsFileControls.hpp>

#include <controls/other/InitControls.hpp>
#include <controls/other/OthersControls.hpp>
#include <controls/other/VerControls.hpp>

#include <controls/other/dialog/NameControls.hpp>

#include <controls/disk/dialog/CantFindFileControls.hpp>
#include <controls/disk/dialog/DeleteAllFilesControls.hpp>
#include <controls/disk/dialog/DeleteFileControls.hpp>
#include <controls/disk/dialog/DeleteFolderControls.hpp>
#include <controls/disk/dialog/FileAlreadyExistsControls.hpp>

#include <controls/midisync/SyncControls.hpp>

#include <controls/misc/PunchControls.hpp>

#include <controls/misc/SecondSeqControls.hpp>
#include <controls/misc/TransControls.hpp>
#include <controls/misc/window/TransposePermanentControls.hpp>

#include <controls/mixer/MixerControls.hpp>
#include <controls/mixer/MixerSetupControls.hpp>
#include <controls/mixer/SelectDrumMixerControls.hpp>
#include <controls/mixer/FxEditControls.hpp>
#include <controls/mixer/window/ChannelSettingsControls.hpp>

#include <controls/vmpc/AudioControls.hpp>

#include <controls/vmpc/AudioMidiDisabledControls.hpp>
#include <controls/vmpc/BufferSizeControls.hpp>
#include <controls/vmpc/DirectToDiskRecorderControls.hpp>
#include <controls/vmpc/MidiControls.hpp>
#include <controls/vmpc/RecordJamControls.hpp>
#include <controls/vmpc/RecordingFinishedControls.hpp>
#include <controls/vmpc/VmpcDiskControls.hpp>

using namespace mpc::controls;

Controls::Controls(mpc::Mpc* mpc)
{
	
	pressedPadVelos = std::vector<int>(16);

	controls["release"] = new GlobalReleaseControls(mpc);

	controls["sequencer"] = new sequencer::SequencerControls(mpc);
	controls["barcopy"] = new sequencer::BarCopyControls(mpc);
	controls["edit"] = new sequencer::EditSequenceControls(mpc);
	controls["nextseq"] = new sequencer::NextSeqControls(mpc);
	controls["nextseqpad"] = new sequencer::NextSeqPadControls(mpc);
	controls["song"] = new sequencer::SongControls(mpc);
	controls["trmove"] = new sequencer::TrMoveControls(mpc);
	controls["trackmute"] = new sequencer::TrMuteControls(mpc);
	controls["user"] = new sequencer::UserControls(mpc);
	controls["assign"] = new sequencer::AssignControls(mpc);
	controls["trmove"] = new sequencer::TrMoveControls(mpc);
	controls["trackmute"] = new sequencer::TrMuteControls(mpc);
	controls["user"] = new sequencer::UserControls(mpc);
	controls["assign"] = new sequencer::AssignControls(mpc);

	auto sec = new sequencer::StepEditorControls(mpc);
	controls["sequencer_step"] = sec;
	controls["step_tc"] = sec;

	controls["changebars"] = new sequencer::window::ChangeBarsControls(mpc);
	controls["changebars2"] = new sequencer::window::ChangeBars2Controls(mpc);
	controls["sequence"] = new sequencer::window::SequenceControls(mpc);
	controls["timedisplay"] = new sequencer::window::TimeDisplayControls(mpc);
	controls["tempochange"] = new sequencer::window::TempoChangeControls(mpc);
	controls["timingcorrect"] = new sequencer::window::TimingCorrectControls(mpc);
	controls["loopbarswindow"] = new sequencer::window::LoopBarsControls(mpc);
	controls["editvelocity"] = new sequencer::window::EditVelocityControls(mpc);
	controls["midioutput"] = new sequencer::window::MidiOutputControls(mpc);
	controls["midiinput"] = new sequencer::window::MidiInputControls(mpc);
	controls["multirecordingsetup"] = new sequencer::window::MultiRecordingSetupControls(mpc);
	controls["transmitprogramchanges"] = new sequencer::window::TransmitProgramChangesControls(mpc);
	controls["eraseallofftracks"] = new sequencer::window::EraseAllOffTracksControls(mpc);
	controls["track"] = new sequencer::window::TrackControls(mpc);
	controls["countmetronome"] = new sequencer::window::CountMetronomeControls(mpc);
	controls["changetsig"] = new sequencer::window::ChangeTsigControls(mpc);
	controls["editmultiple"] = new sequencer::window::EditMultipleControls(mpc);
	controls["insertevent"] = new sequencer::window::InsertEventControls(mpc);
	controls["pasteevent"] = new sequencer::window::PasteEventControls(mpc);
	controls["assign16levels"] = new sequencer::window::Assign16LevelsControls(mpc);
	controls["midiinputmonitor"] = new sequencer::window::MidiMonitorControls(mpc);
	controls["midioutputmonitor"] = new sequencer::window::MidiMonitorControls(mpc);
	controls["erase"] = new sequencer::window::EraseControls(mpc);

	controls["metronomesound"] = new sequencer::dialog::MetronomeSoundControls(mpc);
	controls["copytrack"] = new sequencer::dialog::CopyTrackControls(mpc);
	controls["copysequence"] = new sequencer::dialog::CopySequenceControls(mpc);
	controls["deletealltracks"] = new sequencer::dialog::DeleteAllTracksControls(mpc);
	controls["deleteallsequences"] = new sequencer::dialog::DeleteAllSequencesControls(mpc);
	controls["deletetrack"] = new sequencer::dialog::DeleteTrackControls(mpc);
	controls["deletesequence"] = new sequencer::dialog::DeleteSequenceControls(mpc);

	controls["trim"] = new sampler::TrimControls(mpc);
	controls["loop"] = new sampler::LoopControls(mpc);
	controls["zone"] = new sampler::ZoneControls(mpc);
	controls["numberofzones"] = new sampler::window::NumberOfZonesControls(mpc);
	controls["params"] = new sampler::SndParamsControls(mpc);
	controls["drum"] = new sampler::DrumControls(mpc);
	controls["purge"] = new sampler::PurgeControls(mpc);
	controls["editsound"] = new sampler::window::EditSoundControls(mpc);

	controls["programassign"] = new sampler::PgmAssignControls(mpc);
	controls["programparams"] = new sampler::PgmParamsControls(mpc);
	controls["sample"] = new sampler::SampleControls(mpc);
	controls["initpadassign"] = new sampler::InitPadAssignControls(mpc);

	controls["keeporretry"] = new sampler::window::KeepOrRetryControls(mpc);
	controls["assignmentview"] = new sampler::window::AssignmentViewControls(mpc);
	controls["program"] = new sampler::window::ProgramControls(mpc);
	controls["copynoteparameters"] = new sampler::window::CopyNoteParametersControls(mpc);
	controls["veloenvfilter"] = new sampler::window::VeloEnvFilterControls(mpc);
	controls["muteassign"] = new sampler::window::MuteAssignControls(mpc);
	controls["velopitch"] = new sampler::window::VeloPitchControls(mpc);
	controls["velocitymodulation"] = new sampler::window::VelocityModulationControls(mpc);
	controls["sound"] = new sampler::window::SoundControls(mpc);
	controls["autochromaticassignment"] = new sampler::window::AutoChromaticAssignmentControls(mpc);

	controls["createnewprogram"] = new sampler::dialog::CreateNewProgramControls(mpc);
	controls["copyprogram"] = new sampler::dialog::CopyProgramControls(mpc);
	controls["deleteallprograms"] = new sampler::dialog::DeleteAllProgramsControls(mpc);
	controls["deleteprogram"] = new sampler::dialog::DeleteProgramControls(mpc);
	controls["deletesound"] = new sampler::dialog::DeleteSoundControls(mpc);
	controls["deleteallsound"] = new sampler::dialog::DeleteAllSoundControls(mpc);
	controls["convertsound"] = new sampler::dialog::ConvertSoundControls(mpc);
	controls["monotostereo"] = new sampler::dialog::MonoToStereoControls(mpc);
	controls["copysound"] = new sampler::dialog::CopySoundControls(mpc);
	controls["resample"] = new sampler::dialog::ResampleControls(mpc);
	controls["stereotomono"] = new sampler::dialog::StereoToMonoControls(mpc);

	auto zc = new sampler::window::ZoomControls(mpc);
	controls["startfine"] = zc;
	controls["endfine"] = zc;
	controls["looptofine"] = zc;
	controls["loopendfine"] = zc;
	controls["zonestartfine"] = zc;
	controls["zoneendfine"] = zc;

	controls["selectdrum"] = new sampler::SelectDrumControls(mpc);

	controls["load"] = new disk::LoadControls(mpc);
	controls["save"] = new disk::SaveControls(mpc);
	controls["format"] = new disk::FormatControls(mpc);
	controls["setup"] = new disk::SetupControls(mpc);

	controls["loadasequence"] = new disk::window::LoadASequenceControls(mpc);
	controls["saveasound"] = new disk::window::SaveASoundControls(mpc);
	controls["mpc2000xlallfile"] = new disk::window::MPC2000XLAllFileControls(mpc);
	controls["loadasequencefromall"] = new disk::window::LoadASequenceFromAllControls(mpc);
	controls["loadapsfile"] = new disk::window::LoadApsFileControls(mpc);
	controls["saveallfile"] = new disk::window::SaveAllFileControls(mpc);
	controls["loadaprogram"] = new disk::window::LoadAProgramControls(mpc);
	controls["directory"] = new disk::window::DirectoryControls(mpc);
	controls["saveasequence"] = new disk::window::SaveASequenceControls(mpc);
	controls["saveaprogram"] = new disk::window::SaveAProgramControls(mpc);
	controls["saveapsfile"] = new disk::window::SaveApsFileControls(mpc);
	controls["loadasound"] = new disk::window::LoadASoundControls(mpc);

	controls["name"] = new other::dialog::NameControls(mpc);

	controls["others"] = new other::OthersControls(mpc);
	controls["init"] = new other::InitControls(mpc);
	controls["ver"] = new other::VerControls(mpc);

	controls["deleteallfiles"] = new disk::dialog::DeleteAllFilesControls(mpc);
	controls["deletefile"] = new disk::dialog::DeleteFileControls(mpc);
	controls["deletefolder"] = new disk::dialog::DeleteFolderControls(mpc);
	controls["cantfindfile"] = new disk::dialog::CantFindFileControls(mpc);
	controls["filealreadyexists"] = new disk::dialog::FileAlreadyExistsControls(mpc);

	controls["punch"] = new misc::PunchControls(mpc);
	controls["trans"] = new misc::TransControls(mpc);
	controls["2ndseq"] = new misc::SecondSeqControls(mpc);

	controls["transposepermanent"] = new misc::window::TransposePermanentControls(mpc);

	
	controls["selectdrum_mixer"] = new mixer::SelectDrumMixerControls(mpc);
	controls["mixerv2"] = new mixer::MixerControls(mpc);
	controls["mixersetup"] = new mixer::MixerSetupControls(mpc);
	controls["fxedit"] = new mixer::FxEditControls(mpc);
	controls["channelsettings"] = new mixer::window::ChannelSettingsControls(mpc);

	controls["sync"] = new midisync::SyncControls(mpc);

	controls["disk"] = new vmpc::VmpcDiskControls(mpc);
	controls["midi"] = new vmpc::MidiControls(mpc);
	controls["audio"] = new vmpc::AudioControls(mpc);
	controls["buffersize"] = new vmpc::BufferSizeControls(mpc);
	controls["audiomididisabled"] = new vmpc::AudioMidiDisabledControls(mpc);
	controls["directtodiskrecorder"] = new vmpc::DirectToDiskRecorderControls(mpc);
	controls["recordjam"] = new vmpc::RecordJamControls(mpc);
	controls["recordingfinished"] = new vmpc::RecordingFinishedControls(mpc);
}

void Controls::setCtrlPressed(bool b) {
	ctrlPressed = b;
}

void Controls::setAltPressed(bool b) {
	altPressed = b;
}

bool Controls::isCtrlPressed() {
	return ctrlPressed;
}

bool Controls::isAltPressed() {
	return altPressed;
}

bool Controls::isErasePressed() {
	return erasePressed;
}

bool Controls::isRecPressed() {
	return recPressed;
}

bool Controls::isOverDubPressed() {
	return overDubPressed;
}

bool Controls::isTapPressed() {
	return tapPressed;
}

bool Controls::isGoToPressed() {
	return goToPressed;
}

bool Controls::isShiftPressed() {
	return shiftPressed;
}

bool Controls::isF3Pressed() {
	return f3Pressed;
}

bool Controls::isF4Pressed() {
	return f4Pressed;
}

bool Controls::isF5Pressed() {
	return f5Pressed;
}

bool Controls::isF6Pressed() {
	return f6Pressed;
}

std::set<int>* Controls::getPressedPads() {
	return &pressedPads;
}

std::vector<int>* Controls::getPressedPadVelos() {
	return &pressedPadVelos;
}

void Controls::setErasePressed(bool b) {
	erasePressed = b;
}

void Controls::setRecPressed(bool b) {
	recPressed = b;
}

void Controls::setOverDubPressed(bool b) {
	overDubPressed = b;
}

void Controls::setTapPressed(bool b) {
	tapPressed = b;
}

void Controls::setGoToPressed(bool b) {
	goToPressed = b;
}

void Controls::setShiftPressed(bool b) {
	shiftPressed = b;
}

void Controls::setF3Pressed(bool b) {
	f3Pressed = b;
}

void Controls::setF4Pressed(bool b) {
	f4Pressed = b;
}

void Controls::setF5Pressed(bool b) {
	f5Pressed = b;
}

void Controls::setF6Pressed(bool b) {
	f6Pressed = b;
}

AbstractControls* Controls::getControls(std::string s)
{
	return controls[s];
}

GlobalReleaseControls* Controls::getReleaseControls()
{
	return (GlobalReleaseControls*) controls["release"];
}

Controls::~Controls()
{
}
