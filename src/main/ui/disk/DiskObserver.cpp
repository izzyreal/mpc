#include <ui/disk/DiskObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DiskWindowGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>
#include <file/File.hpp>

using namespace mpc::ui::disk;
using namespace std;

DiskObserver::DiskObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	views = vector<string>{ "All Files", ".SND", ".PGM", ".APS", ".MID", ".ALL", ".WAV", ".SEQ", ".SET" };
	
	types = vector<string>{ "Save All Sequences & Songs", "Save a Sequence", "Save All Program and Sounds", "Save a Program & Sounds", "Save a Sound", "Copy Operating System" };
	pgmSaveNames = vector<string>{ "PROGRAM ONLY", "WITH SOUNDS", "WITH .WAV" };
	apsSaveNames = vector<string>{ "APS ONLY", "WITH SOUNDS", "WITH .WAV" };
	auto uis = mpc->getUis().lock();
	samplerGui = uis->getSamplerGui();
	soundGui = uis->getSoundGui();
	diskWindowGui = uis->getDiskWindowGui();
	diskWindowGui->addObserver(this);
	samplerGui->addObserver(this);
	soundGui->addObserver(this);
	disk = mpc->getDisk();
	sampler = mpc->getSampler();
	sequencer = mpc->getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->addObserver(this);
	diskGui = mpc->getUis().lock()->getDiskGui();
	diskGui->addObserver(this);
	auto lSampler = sampler.lock();
	int activeTrack = lSequencer->getActiveTrackIndex();
	int drum = lSequencer->getActiveSequence().lock()->getTrack(activeTrack).lock()->getBusNumber() - 1;
	if (!mpc->getAudioMidiServices().lock()->isDisabled()) {
		int candidate = drum;
		if (candidate < 0)
			candidate = 0;

		mpcSoundPlayerChannel = lSampler->getDrum(candidate);
		mpcSoundPlayerChannel->addObserver(this);
		program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	}

	csn = mpc->getLayeredScreen().lock()->getCurrentScreenName();

	auto lDisk = disk.lock();
	
	if (lDisk) {
		lDisk->addObserver(this);
	}
	
	auto ls = mpc->getLayeredScreen().lock();
	
	if (csn.compare("load") == 0) {
		viewField = ls->lookupField("view");
		directoryField = ls->lookupField("directory");
		fileField = ls->lookupField("file");
		sizeLabel = ls->lookupLabel("size");
		freesndLabel = ls->lookupLabel("freesnd");
		freeseqLabel = ls->lookupLabel("freeseq");
		displayView();
		if (lDisk) {
			displayDirectory();
			displayFile();
			displaySize();
		}
		displayFreeSnd();
		freeseqLabel.lock()->setText("  2640K");
	}
	else if (csn.compare("save") == 0) {
		typeField = ls->lookupField("type");
		fileField = ls->lookupField("file");
		deviceField = ls->lookupField("device");
		sizeLabel = ls->lookupLabel("size");
		freeLabel = ls->lookupLabel("free");
		displaySize();
		displayType();
		displayFile();
		displayDevice();
		displayFree();
	}
	else if (csn.compare("loadaprogram") == 0) {
		loadReplaceSoundField = ls->lookupField("loadreplacesound");
		displayLoadReplaceSound();
	}
	else if (csn.compare("saveaprogram") == 0) {
		fileLabel = ls->lookupLabel("file");
		saveField = ls->lookupField("save");
		replaceSameSoundsField = ls->lookupField("replacesamesounds");
		displayFile();
		displaySave();
		displayReplaceSameSounds();
	}
	else if (csn.compare("saveapsfile") == 0) {
		fileField = ls->lookupField("file");
		saveField = ls->lookupField("save");
		replaceSameSoundsField = ls->lookupField("replacesamesounds");
		displayFile();
		displaySave();
		displayReplaceSameSounds();
	}
	else if (csn.compare("loadasound") == 0) {
		loadFileNameLabel = ls->lookupLabel("loadsoundfilename");
		loadFileNameLabel.lock()->setText("File:" + diskGui->getSelectedFileName());
		assignToNoteField = ls->lookupField("assigntonote");
		displayAssignToNote();
	}
	else if (csn.compare("saveasound") == 0) {
		fileField = ls->lookupField("file");
		fileTypeField = ls->lookupField("filetype");
		displayFile();
		displayFileType();
	}
	else if (csn.compare("loadasequence") == 0) {
		fileLabel = ls->lookupLabel("file");
		loadIntoField = ls->lookupField("loadinto");
		nameLabel = ls->lookupLabel("name");
		displayFile();
		displayLoadInto();
	}
	else if (csn.compare("saveasequence") == 0) {
		saveAsField = ls->lookupField("saveas");
		fileField = ls->lookupField("file");
		fileLabel = ls->lookupLabel("file1");
		displaySaveAs();
		displayFile();
	}
	else if (csn.compare("cantfindfile") == 0) {
		fileField = ls->lookupField("file");
		fileField.lock()->setText(*diskGui->getCannotFindFileName());
	}
}

void DiskObserver::displaySaveAs()
{
	saveAsField.lock()->setText("MIDI FILE TYPE " + to_string(diskGui->getSaveSequenceAs()));
}

void DiskObserver::displayFileType()
{
	fileTypeField.lock()->setText(diskGui->getFileTypeSaveSound() == 0 ? "MPC2000" : "WAV");
}

void DiskObserver::displaySave()
{
	if (csn.compare("saveaprogram") == 0) {
		saveField.lock()->setText(pgmSaveNames[diskGui->getPgmSave()]);
	}
	else if (csn.compare("saveapsfile") == 0) {
		saveField.lock()->setText(apsSaveNames[diskGui->getPgmSave()]);
	}
}

void DiskObserver::displayReplaceSameSounds()
{
	replaceSameSoundsField.lock()->setText(diskGui->getSaveReplaceSameSounds() ? "YES" : "NO");
}

void DiskObserver::displayFree()
{
	//auto space = stringBuilder().append("")->append(npc((*::java::io::File::listRoots())[0])->getFreeSpace())->toString();
	//if(npc(space)->length() > 9)
   //     space = stringBuilder().append(npc(space)->substring(0, 3))->append("GB")->toString();

	freeLabel.lock()->setText("-1GB");
}

void DiskObserver::displayDevice()
{
	deviceField.lock()->setText("SCSI-1");
}

void DiskObserver::displayFreeSnd()
{
	freesndLabel.lock()->setText(" " + moduru::lang::StrUtil::padLeft(to_string(sampler.lock()->getFreeSampleSpace() / int(1000)), " ", 5) + "K");
}

void DiskObserver::displayAssignToNote()
{
	auto nn = samplerGui->getNote();
	auto pn = program.lock()->getPadNumberFromNote(nn);
	auto padName = pn == -1 ? "OFF" : sampler.lock()->getPadName(pn);
	auto noteName = nn == 34 ? "--" : to_string(nn);
	assignToNoteField.lock()->setText(noteName + "/" + padName);
}

void DiskObserver::displaySize()
{
	auto lSizeLabel = sizeLabel.lock();
	if (csn.compare("load") == 0) {
		if (disk.lock()->getFileNames().size() == 0) {
			lSizeLabel->setText("      K");
			return;
		}
		lSizeLabel->setText(to_string(diskGui->getFileSize(diskGui->getFileLoad())) + "K");
	}

	if (csn.compare("save") == 0) {
		auto lSequencer = sequencer.lock();
		auto seq = lSequencer->getActiveSequence().lock();
		auto size = 0;
		auto lSampler = sampler.lock();
		switch (diskGui->getType()) {
		case 0:
			size = lSequencer->getUsedSequenceCount() * 25;
			break;
		case 1:
			size = seq->isUsed() ? 10 + static_cast<int >(seq->getEventCount() / 1000) : -1;
			break;
		case 2:
			size = lSampler->getProgramCount() * 4;
			break;
		case 3:
			size = 4;
			break;
		case 4:
			size = lSampler->getSoundCount() == 0 ? -1 : (lSampler->getSound(soundGui->getSoundIndex()).lock()->getSampleData()->size() * 2 / 1000);
			break;
		case 5:
			size = 512;
			break;
		}

		lSizeLabel->setText(moduru::lang::StrUtil::padLeft(to_string(size == -1 ? 0 : size), " ", 6) + "K");
	}
}

void DiskObserver::displayLoadInto()
{
	auto seqn = diskWindowGui->getLoadInto();
	loadIntoField.lock()->setTextPadded(seqn + 1, "0");
	nameLabel.lock()->setText("-" + sequencer.lock()->getSequence(seqn).lock()->getName());
}

void DiskObserver::displayFile()
{
	auto lSampler = sampler.lock();
	auto lDisk = disk.lock();
	auto lFileField = fileField.lock();
	auto lFileLabel = fileLabel.lock();
	auto nameGui = mpc->getUis().lock()->getNameGui();
	if (csn.compare("saveapsfile") == 0) {
		lFileField->setText(nameGui->getName());
	}
	else if (csn.compare("saveasound") == 0) {
		lFileField->setText(nameGui->getName());
	}
	else if (csn.compare("saveaprogram") == 0) {
		lFileLabel->setText(nameGui->getName() + ".PGM");
		return;
	}
	else if (csn.compare("loadasequence") == 0) {
		auto s = sequencer.lock()->getPlaceHolder().lock();
		lFileLabel->setText("File:" + moduru::lang::StrUtil::toUpper(mpc::disk::AbstractDisk::padFileName16(s->getName())) + ".MID");
		return;
	}
	else if (csn.compare("saveasequence") == 0) {
		auto name = nameGui->getName();
		lFileField->setText(name.substr(0, 1));
		lFileLabel->setText(name.substr(1));
	}
	else if (csn.compare("load") == 0) {
		if (lDisk->getFileNames().size() == 0) {
			lFileField->setText("");
			return;
		}
		string selectedFileName = diskGui->getSelectedFileName();
		auto selectedFile = diskGui->getSelectedFile();
		if (selectedFileName.length() != 0 && selectedFile != nullptr && selectedFile->isDirectory()) {
			lFileField->setText(u8"\u00C3" + moduru::lang::StrUtil::padRight(moduru::file::FileUtil::splitName(selectedFileName)[0], " ", 16));
		}
		else {
			lFileField->setText(padExtensionRight(selectedFileName));
		}
	}
	else  if (csn.compare("save") == 0) {
		string file = "";

		string num;
		string name;
		auto lSequencer = sequencer.lock();
		auto seq = lSequencer->getActiveSequence().lock();
		switch (diskGui->getType()) {
		case 0:
			file = "ALL_SEQ_SONG1";
			break;
		case 1:
			num = moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2);
			name = seq->getName();
			file = num + "-" + name;
			break;
		case 2:
			file = "ALL_PROGRAM";
			break;
		case 3:
			file = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock())->getName();
			break;
		case 4:
			file = lSampler->getSoundCount() == 0 ? "" : dynamic_pointer_cast<mpc::sampler::Sound>(lSampler->getSound(soundGui->getSoundIndex()).lock())->getName();
			break;
		}
		lFileField->setText(file);
	}
}

void DiskObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = "";
	try {
		param = nonstd::any_cast<string>(a);
	}
	catch (std::exception& e) {
		return;
	}

	if (param.compare("savesequenceas") == 0) {
		displaySaveAs();
	}
	else if (param.compare("filetype") == 0) {
		displayFileType();
	}
	else if (param.compare("pgm") == 0) {
		displayFile();
	}
	else if (param.compare("save") == 0) {
		displaySave();
	}
	else if (param.compare("savereplacesamesounds") == 0) {
		displayReplaceSameSounds();
	}
	else if (param.compare("loadinto") == 0) {
		displayLoadInto();
	}
	else if (param.compare("directory") == 0) {
		displayDirectory();
	}
	else if (param.compare("view") == 0) {
		displayView();
		displayDirectory();
		displayFile();
		displaySize();
	}
	else if (param.compare("type") == 0 ||
		param.compare("fileselect") == 0 || param.compare("filesave") == 0) {
		if (param.compare("type") == 0) displayType();
		displayFile();
		displaySize();
	}
	else if (param.compare("loadreplacesound") == 0) {
		displayLoadReplaceSound();
	}
	else if (param.compare("removepopup") == 0) {
		mpc->getLayeredScreen().lock()->removePopup();
	}

	else if (param.compare("padandnote") == 0 || param.compare("note") == 0) {
		if (csn.compare("loadasound") == 0)
			displayAssignToNote();
	}
	else if (param.compare("seqnumbername") == 0 || param.compare("soundnumber") == 0) {
		displayFile();
		displaySize();
	}
}

void DiskObserver::displayType()
{
	typeField.lock()->setText(types[diskGui->getType()]);
}

void DiskObserver::displayLoadReplaceSound()
{
	loadReplaceSoundField.lock()->setText(diskGui->getLoadReplaceSound() ? "YES" : "NO(FASTER)");
}

void DiskObserver::displayView()
{
	viewField.lock()->setText(views[diskGui->getView()]);
}

void DiskObserver::displayDirectory()
{
	auto lDisk = disk.lock();
	directoryField.lock()->setText(lDisk->getDirectoryName());
}

string DiskObserver::padExtensionRight(string s)
{
	auto periodIndex = s.find_last_of(".");
	if (periodIndex == string::npos) return s;
	auto extension = s.substr(periodIndex, s.length());
	auto fileName = moduru::lang::StrUtil::padRight(s.substr(0, periodIndex), " ", 16);
	return fileName + extension;
}

DiskObserver::~DiskObserver() {
	auto lDisk = disk.lock();
	lDisk->deleteObserver(this);
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObserver(this);
	diskWindowGui->deleteObserver(this);
	samplerGui->deleteObserver(this);
	soundGui->deleteObserver(this);
	diskGui->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
}
