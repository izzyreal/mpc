#include "Controls.hpp"

#include <Mpc.hpp>

#include <controls/BaseControls.hpp>

#include <controls/GlobalReleaseControls.hpp>

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
#include <controls/sampler/window/ProgramControls.hpp>
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

#include <controls/vmpc/DirectToDiskRecorderControls.hpp>
#include <controls/vmpc/RecordJamControls.hpp>
#include <controls/vmpc/RecordingFinishedControls.hpp>
#include <controls/vmpc/VmpcDiskControls.hpp>
#include <controls/vmpc/VmpcSettingsControls.hpp>

using namespace mpc::controls;

Controls::Controls()
{
	
	pressedPadVelos = std::vector<int>(16);

	controls["release"] = new GlobalReleaseControls();

	controls["editsound"] = new sampler::window::EditSoundControls();

	controls["keeporretry"] = new sampler::window::KeepOrRetryControls();
	controls["assignmentview"] = new sampler::window::AssignmentViewControls();
	controls["program"] = new sampler::window::ProgramControls();
	controls["copynoteparameters"] = new sampler::window::CopyNoteParametersControls();
	controls["veloenvfilter"] = new sampler::window::VeloEnvFilterControls();
	controls["muteassign"] = new sampler::window::MuteAssignControls();
	controls["velopitch"] = new sampler::window::VeloPitchControls();
	controls["velocitymodulation"] = new sampler::window::VelocityModulationControls();
	controls["autochromaticassignment"] = new sampler::window::AutoChromaticAssignmentControls();

	controls["createnewprogram"] = new sampler::dialog::CreateNewProgramControls();
	controls["copyprogram"] = new sampler::dialog::CopyProgramControls();
	controls["deleteallprograms"] = new sampler::dialog::DeleteAllProgramsControls();
	controls["deleteprogram"] = new sampler::dialog::DeleteProgramControls();
	controls["deletesound"] = new sampler::dialog::DeleteSoundControls();
	controls["deleteallsound"] = new sampler::dialog::DeleteAllSoundControls();
	controls["convertsound"] = new sampler::dialog::ConvertSoundControls();
	controls["monotostereo"] = new sampler::dialog::MonoToStereoControls();
	controls["copysound"] = new sampler::dialog::CopySoundControls();
	controls["resample"] = new sampler::dialog::ResampleControls();
	controls["stereotomono"] = new sampler::dialog::StereoToMonoControls();

	controls["startfine"] = new sampler::window::ZoomControls();
	controls["endfine"] = new sampler::window::ZoomControls();
	controls["looptofine"] = new sampler::window::ZoomControls();
	controls["loopendfine"] = new sampler::window::ZoomControls();
	controls["zonestartfine"] = new sampler::window::ZoomControls();
	controls["zoneendfine"] = new sampler::window::ZoomControls();

	controls["load"] = new disk::LoadControls();
	controls["save"] = new disk::SaveControls();
	controls["format"] = new disk::FormatControls();
	controls["setup"] = new disk::SetupControls();

	controls["loadasequence"] = new disk::window::LoadASequenceControls();
	controls["saveasound"] = new disk::window::SaveASoundControls();
	controls["mpc2000xlallfile"] = new disk::window::MPC2000XLAllFileControls();
	controls["loadasequencefromall"] = new disk::window::LoadASequenceFromAllControls();
	controls["loadapsfile"] = new disk::window::LoadApsFileControls();
	controls["saveallfile"] = new disk::window::SaveAllFileControls();
	controls["loadaprogram"] = new disk::window::LoadAProgramControls();
	controls["directory"] = new disk::window::DirectoryControls();
	controls["saveasequence"] = new disk::window::SaveASequenceControls();
	controls["saveaprogram"] = new disk::window::SaveAProgramControls();
	controls["saveapsfile"] = new disk::window::SaveApsFileControls();
	controls["loadasound"] = new disk::window::LoadASoundControls();

	controls["name"] = new other::dialog::NameControls();

	controls["init"] = new other::InitControls();
	controls["ver"] = new other::VerControls();

	controls["deleteallfiles"] = new disk::dialog::DeleteAllFilesControls();
	controls["deletefile"] = new disk::dialog::DeleteFileControls();
	controls["deletefolder"] = new disk::dialog::DeleteFolderControls();
	controls["cantfindfile"] = new disk::dialog::CantFindFileControls();
	controls["filealreadyexists"] = new disk::dialog::FileAlreadyExistsControls();

	controls["punch"] = new misc::PunchControls();
	controls["trans"] = new misc::TransControls();
	controls["2ndseq"] = new misc::SecondSeqControls();

	controls["transposepermanent"] = new misc::window::TransposePermanentControls();

	controls["selectdrum_mixer"] = new mixer::SelectDrumMixerControls();
	controls["mixer"] = new mixer::MixerControls();
	controls["mixersetup"] = new mixer::MixerSetupControls();
	controls["fxedit"] = new mixer::FxEditControls();
	controls["channelsettings"] = new mixer::window::ChannelSettingsControls();

	controls["sync"] = new midisync::SyncControls();

	controls["disk"] = new vmpc::VmpcDiskControls();
	controls["directtodiskrecorder"] = new vmpc::DirectToDiskRecorderControls();
	controls["recordjam"] = new vmpc::RecordJamControls();
	controls["recordingfinished"] = new vmpc::RecordingFinishedControls();
	controls["settings"] = new vmpc::VmpcSettingsControls();
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

BaseControls* Controls::getControls(std::string s)
{
	return controls[s];
}

GlobalReleaseControls* Controls::getReleaseControls()
{
	return (GlobalReleaseControls*) controls["release"];
}

Controls::~Controls()
{
	for (auto c : controls)
	{
		delete c.second;
	}
}
