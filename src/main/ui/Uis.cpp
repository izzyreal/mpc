#include <ui/Uis.hpp>

#include <ui/NameGui.hpp>

#include <ui/other/OthersGui.hpp>

#include <ui/misc/PunchGui.hpp>
#include <ui/misc/SecondSeqGui.hpp>
#include <ui/misc/TransGui.hpp>

#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>

#include <ui/midisync/MidiSyncGui.hpp>

#include <ui/sequencer/SongGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>

#include <ui/sequencer/window/Assign16LevelsGui.hpp>
#include <ui/sequencer/window/EraseGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>

#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/MixerGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>

#include <ui/sampler/window/ZoomGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>

#include <ui/vmpc/DeviceGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>

using namespace mpc::ui;
using namespace std;

Uis::Uis() 
{
	if (noteNames.size() != 128) {
		noteNames = vector<string>(128);
		int octave = -2;
		int noteCounter = 0;
		
		for (int j = 0; j < 128; j++)
		{
			string octaveString = to_string(octave);
		
			if (octave == -2)
			{
				octaveString = u8"\u00D2";
			}

			if (octave == -1)
			{
				octaveString = u8"\u00D3";
			}

			noteNames[j] = someNoteNames[noteCounter] + octaveString;
			noteCounter++;
			
			if (noteCounter == 12)
			{
				noteCounter = 0;
				octave++;
			}

		}
	}

	stepEditorGui = new mpc::ui::sequencer::StepEditorGui();
	editSequenceGui = new mpc::ui::sequencer::EditSequenceGui();
	songGui = new mpc::ui::sequencer::SongGui();
	barCopyGui = new mpc::ui::sequencer::BarCopyGui();
	trMoveGui = new mpc::ui::sequencer::TrMoveGui();

	eraseGui = new mpc::ui::sequencer::window::EraseGui();
	sequencerWindowGui = new mpc::ui::sequencer::window::SequencerWindowGui();
	assign16LevelsGui = new mpc::ui::sequencer::window::Assign16LevelsGui();

	samplerGui = new mpc::ui::sampler::SamplerGui();
	nameGui = new mpc::ui::NameGui();
	soundGui = new mpc::ui::sampler::SoundGui();

	mixerGui = new mpc::ui::sampler::MixerGui();
	mixerSetupGui = new mpc::ui::sampler::MixerSetupGui();

	editSoundGui = new mpc::ui::sampler::window::EditSoundGui();
	zoomGui = new mpc::ui::sampler::window::ZoomGui();
	samplerWindowGui = new mpc::ui::sampler::window::SamplerWindowGui();

	diskGui = new mpc::ui::disk::DiskGui();
	directoryGui = new mpc::ui::disk::window::DirectoryGui(diskGui);
	diskWindowGui = new mpc::ui::disk::window::DiskWindowGui();

	midiSyncGui = new mpc::ui::midisync::MidiSyncGui();

	punchGui = new mpc::ui::misc::PunchGui();
	transGui = new mpc::ui::misc::TransGui();
	secondSeqGui = new mpc::ui::misc::SecondSeqGui();

	d2dRecorderGui = new mpc::ui::vmpc::DirectToDiskRecorderGui();
	deviceGui = new mpc::ui::vmpc::DeviceGui();

	othersGui = new mpc::ui::other::OthersGui();
}
vector<string> Uis::noteNames;

mpc::ui::sequencer::window::Assign16LevelsGui* Uis::getAssign16LevelsGui()
{
	return assign16LevelsGui;
}

mpc::ui::sequencer::StepEditorGui* Uis::getStepEditorGui()
{
	return stepEditorGui;
}

mpc::ui::sampler::MixerGui* Uis::getMixerGui()
{
	return mixerGui;
}

mpc::ui::sequencer::EditSequenceGui* Uis::getEditSequenceGui()
{
	return editSequenceGui;
}

mpc::ui::sequencer::window::SequencerWindowGui* Uis::getSequencerWindowGui()
{
	return sequencerWindowGui;
}

mpc::ui::midisync::MidiSyncGui* Uis::getMidiSyncGui()
{
	return midiSyncGui;
}

mpc::ui::sequencer::BarCopyGui* Uis::getBarCopyGui()
{
	return barCopyGui;
}

mpc::ui::sequencer::TrMoveGui* Uis::getTrMoveGui()
{
	return trMoveGui;
}

mpc::ui::sampler::SoundGui* Uis::getSoundGui()
{
	return soundGui;
}

mpc::ui::sampler::window::ZoomGui* Uis::getZoomGui()
{
	return zoomGui;
}

mpc::ui::disk::DiskGui* Uis::getDiskGui()
{
	return diskGui;
}

mpc::ui::disk::window::DirectoryGui* Uis::getDirectoryGui()
{
	return directoryGui;
}

mpc::ui::disk::window::DiskWindowGui* Uis::getDiskWindowGui()
{
	return diskWindowGui;
}

mpc::ui::sampler::SamplerGui* Uis::getSamplerGui()
{
	return samplerGui;
}

mpc::ui::sampler::window::SamplerWindowGui* Uis::getSamplerWindowGui()
{
	return samplerWindowGui;
}

mpc::ui::NameGui* Uis::getNameGui()
{
	return nameGui;
}

mpc::ui::vmpc::DeviceGui* Uis::getDeviceGui()
{
	return deviceGui;
}

mpc::ui::sequencer::SongGui* Uis::getSongGui()
{
	return songGui;
}

mpc::ui::sampler::MixerSetupGui* Uis::getMixerSetupGui()
{
	return mixerSetupGui;
}

mpc::ui::sequencer::window::EraseGui* Uis::getEraseGui()
{
	return eraseGui;
}

mpc::ui::misc::PunchGui* Uis::getPunchGui()
{
	return punchGui;
}

mpc::ui::other::OthersGui* Uis::getOthersGui()
{
	return othersGui;
}

mpc::ui::vmpc::DirectToDiskRecorderGui* Uis::getD2DRecorderGui()
{
	return d2dRecorderGui;
}

mpc::ui::misc::TransGui* Uis::getTransGui()
{
	return transGui;
}

mpc::ui::misc::SecondSeqGui* Uis::getSecondSeqGui()
{
	return secondSeqGui;
}

mpc::ui::sampler::window::EditSoundGui* Uis::getEditSoundGui() {
	return editSoundGui;
}


Uis::~Uis() {
	if (sequencerWindowGui != nullptr) delete sequencerWindowGui;
	if (assign16LevelsGui != nullptr) delete assign16LevelsGui;
	if (eraseGui != nullptr) delete eraseGui;

	if (stepEditorGui != nullptr) delete stepEditorGui;
	if (editSequenceGui != nullptr) delete editSequenceGui;
	if (barCopyGui != nullptr) delete barCopyGui;
	if (trMoveGui != nullptr) delete trMoveGui;
	if (soundGui != nullptr) delete soundGui;
	if (samplerGui != nullptr) delete samplerGui;
	if (editSoundGui != nullptr) delete editSoundGui;
	if (zoomGui != nullptr) delete zoomGui;
	if (samplerWindowGui != nullptr) delete samplerWindowGui;
	if (mixerGui != nullptr) delete mixerGui;
	if (diskGui != nullptr) delete diskGui;
	if (directoryGui != nullptr) delete directoryGui;
	if (diskWindowGui != nullptr) delete diskWindowGui;
	if (midiSyncGui != nullptr) delete midiSyncGui;
	if (nameGui != nullptr) delete nameGui;
	if (mixerSetupGui != nullptr) delete mixerSetupGui;
	if (punchGui != nullptr) delete punchGui;
	if (d2dRecorderGui != nullptr) delete d2dRecorderGui;
	if (transGui != nullptr) delete transGui;
	if (secondSeqGui != nullptr) delete secondSeqGui;
	if (deviceGui != nullptr) delete deviceGui;
	if (songGui != nullptr) delete songGui;
	if (othersGui != nullptr) delete othersGui;
}
