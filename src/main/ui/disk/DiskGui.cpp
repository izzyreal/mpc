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

int DiskGui::getFileSize(int i)
{
	auto lDisk = Mpc::instance().getDisk().lock();
	if (lDisk->getFile(i) == nullptr || lDisk->getFile(i)->isDirectory()) return 0;
	return (int)(lDisk->getFile(i)->length() / 1024);
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
