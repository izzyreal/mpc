#include <ui/disk/DiskGui.hpp>

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <algorithm>

using namespace mpc::ui::disk;
using namespace std;

DiskGui::DiskGui() 
{
	
}

void DiskGui::setType(int i)
{
    if(i < 0 || i > 5) return;
    type = i;
    setChanged();
    notifyObservers(string("type"));
}

int DiskGui::getType()
{
    return type;
}

mpc::disk::MpcFile* DiskGui::getSelectedFile()
{
	auto lDisk = Mpc::instance().getDisk().lock();
	return lDisk->getFile(fileLoad);
}

string DiskGui::getSelectedFileName()
{
	auto lDisk = Mpc::instance().getDisk().lock();
	return lDisk->getFileNames()[fileLoad];
}

void DiskGui::setDelete(int i)
{
	if (i < 0 || i > 8) return;
	deleteIndex = i;
	setChanged();
	notifyObservers(string("delete"));
}

int DiskGui::getDelete()
{
    return deleteIndex;
}

bool DiskGui::isSelectedFileDirectory()
{
	auto lDisk = Mpc::instance().getDisk().lock();
	return lDisk->getFile(fileLoad)->isDirectory();
}

int DiskGui::getFileSize(int i)
{
	auto lDisk = Mpc::instance().getDisk().lock();
	if (lDisk->getFile(i) == nullptr || lDisk->getFile(i)->isDirectory()) return 0;
	return (int)(lDisk->getFile(i)->length() / 1024);
}

int DiskGui::getView()
{
	return view;
}

void DiskGui::setView(int i)
{
	if (i < 0 || i > 8) return;
	auto lDisk = Mpc::instance().getDisk().lock();
	view = i;
	lDisk->initFiles();
	fileLoad = 0;
	setChanged();
	notifyObservers(string("view"));
}

bool DiskGui::getLoadReplaceSound()
{
    return loadReplaceSound;
}

void DiskGui::setLoadReplaceSound(bool b)
{
    loadReplaceSound = b;
    setChanged();
    notifyObservers(string("loadreplacesound"));
}

int DiskGui::getFileLoad()
{
    return fileLoad;
}

void DiskGui::setSelectedFileNumberLimited(int i)
{
	auto lDisk = Mpc::instance().getDisk().lock();
	if(i < 0 || i + 1 > lDisk->getFileNames().size()) return;
    fileLoad = i;
    setChanged();
    notifyObservers(string("fileselect"));
}

void DiskGui::setFileLoad(int i)
{
	if (i < 0) return;

	fileLoad = i;
	setChanged();
	notifyObservers(string("fileload"));
}

void DiskGui::openPopup(string soundFileName, string ext)
{
    for (auto& c : ext) {
        c = toupper(c);
    }

	Mpc::instance().getLayeredScreen().lock()->createPopup("LOADING " + soundFileName + "." + ext, 85);
}

void DiskGui::removePopup()
{
	Mpc::instance().getLayeredScreen().lock()->removePopup();
}

bool DiskGui::getClearProgramWhenLoading()
{
    return clearProgramWhenLoading;
}

void DiskGui::setClearProgramWhenLoading(bool b)
{
    clearProgramWhenLoading = b;
}

void DiskGui::setWaitingForUser(bool b)
{
    waitingForUser = b;
}

bool DiskGui::isWaitingForUser()
{
    return waitingForUser;
}

void DiskGui::setSkipAll(bool b)
{
    skipAll = b;
}

bool DiskGui::getSkipAll()
{
    return skipAll;
}

void DiskGui::setCannotFindFileName(string s)
{
    cannotFindFileName = s;
}

string* DiskGui::getCannotFindFileName()
{
	return &cannotFindFileName;
}

int DiskGui::getPgmSave()
{
    return save;
}

bool DiskGui::getSaveReplaceSameSounds()
{
    return saveReplaceSameSounds;
}

void DiskGui::setSaveReplaceSameSounds(bool b)
{
    saveReplaceSameSounds = b;
    setChanged();
    notifyObservers(string("savereplacesamesounds"));
}

void DiskGui::setSave(int i)
{
    if(i < 0 || i > 2)
        return;

    save = i;
    setChanged();
    notifyObservers(string("save"));
}

int DiskGui::getFileTypeSaveSound()
{
    return fileTypeSaveSound;
}

void DiskGui::setFileTypeSaveSound(int i)
{
    if(i < 0 || i > 1)
        return;

    fileTypeSaveSound = i;
    setChanged();
    notifyObservers(string("filetype"));
}

int DiskGui::getSaveSequenceAs()
{
    return saveSequenceAs;
}

void DiskGui::setSaveSequenceAs(int i)
{
    if(i < 0 || i > 1)
        return;

    saveSequenceAs = i;
    setChanged();
    notifyObservers(string("savesequenceas"));
}

bool DiskGui::dontAssignSoundWhenLoading()
{
    return dontAssignSoundWhenLoading_;
}

void DiskGui::setDontAssignSoundWhenLoading(bool b)
{
    dontAssignSoundWhenLoading_ = b;
    setChanged();
    notifyObservers(string("padandnote"));
}

void DiskGui::setSequencesFromAllFile(vector<shared_ptr<mpc::sequencer::Sequence>>* sequences)
{
    sequencesFromAllFile = sequences;
}

vector<shared_ptr<mpc::sequencer::Sequence>>* DiskGui::getSequencesFromAllFile()
{
    return sequencesFromAllFile;
}

DiskGui::~DiskGui() {
}
