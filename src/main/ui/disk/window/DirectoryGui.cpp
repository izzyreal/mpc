#include "DirectoryGui.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <ui/disk/DiskGui.hpp>
#include <lcdgui/Field.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

DirectoryGui::DirectoryGui(mpc::Mpc* mpc, mpc::ui::disk::DiskGui* diskGui)
	: mpc(mpc)
	, diskGui(diskGui)
{
}

void DirectoryGui::left()
{
	auto lDisk = mpc->getDisk().lock();
	auto prevDirName = lDisk->getDirectoryName();
	if (xPos == 1) {
		xPos--;
		setChanged();
		notifyObservers(string("focus"));
		return;
	}
	else {
		if (lDisk->moveBack()) {
			lDisk->initFiles();
			diskGui->setFileLoad(0);
			yPos0 = 0;
			for (int i = 0; i < lDisk->getParentFileNames().size(); i++) {
				if (lDisk->getParentFileNames()[i].compare(lDisk->getDirectoryName()) == 0) {
					yOffset0 = i;
					break;
				}
			}
			for (int i = 0; i < lDisk->getFileNames().size(); i++) {
				if (lDisk->getFileNames()[i].compare(prevDirName) == 0) {
					yOffset1 = i;
					diskGui->setFileLoad(i);
					break;
				}
			}
			if (yOffset1 + 1 > lDisk->getFileNames().size())
				yOffset1 = 0;

			if (lDisk->getParentFileNames().size() == 0)
				yOffset0 = 0;

			setChanged();
			notifyObservers(string("disk"));
			setChanged();
			notifyObservers(string("focus"));
		}
	}
}

void DirectoryGui::right()
{
	auto lDisk = mpc->getDisk().lock();
	if (xPos == 0) {
		xPos++;
		setChanged();
		notifyObservers(string("focus"));
		return;
	}
	else {
		if (getSelectedFile() == nullptr || lDisk->getFileNames().size() == 0 || !getSelectedFile()->isDirectory())
			return;

		auto f = getSelectedFile();
		if (f == nullptr)
			return;

		if (!lDisk->moveForward(f->getName()))
			return;

		lDisk->initFiles();
		yPos0 = 0;
		yOffset1 = 0;
		diskGui->setFileLoad(0);
		for (int i = 0; i < lDisk->getParentFileNames().size(); i++) {
			if (lDisk->getParentFileNames()[i].compare(f->getName()) == 0) {
				yOffset0 = i;
				break;
			}
		}
		yPos0 = 0;
		yOffset1 = 0;
		diskGui->setFileLoad(0);
		setChanged();
		notifyObservers(string("disk"));
		setChanged();
		notifyObservers(string("focus"));
	}
}

void DirectoryGui::up()
{
	auto lDisk = mpc->getDisk().lock();
	if (xPos == 0) {
		if (yOffset0 == 0 && yPos0 == 0)
			return;

		if (yPos0 == 0) {
			yOffset0--;
			auto newDirectoryName = lDisk->getParentFileNames()[yOffset0];
			if (lDisk->moveBack()) {
				lDisk->initFiles();
				lDisk->moveForward(newDirectoryName);
				lDisk->initFiles();
				diskGui->setFileLoad(0);
				yOffset1 = 0;
				setChanged();
				notifyObservers(string("disk"));
			}
			return;
		}
		auto newDirectoryName = lDisk->getParentFileNames()[yPos0 - 1 + yOffset0];
		if (lDisk->moveBack()) {
			lDisk->initFiles();
			lDisk->moveForward(newDirectoryName);
			lDisk->initFiles();
			yPos0--;
			yOffset1 = 0;
			diskGui->setFileLoad(0);
			setChanged();
			notifyObservers(string("disk"));
			setChanged();
			notifyObservers(string("focus"));
		}
		return;
	}
	else {
		if (diskGui->getFileLoad() == 0)
			return;

		auto yPos = diskGui->getFileLoad() - yOffset1;
		if (yPos == 0) {
			yOffset1--;
			diskGui->setFileLoad(diskGui->getFileLoad() - 1);
			setChanged();
			notifyObservers(string("right"));
			return;
		}
		diskGui->setFileLoad(diskGui->getFileLoad() - 1);
		setChanged();
		notifyObservers(string("focus"));
	}
}

void DirectoryGui::down()
{
	auto lDisk = mpc->getDisk().lock();
	if (xPos == 0) {
		
		if (lDisk->isRoot()) {
			return;
		}

		if (yOffset0 + yPos0 + 1 >= lDisk->getParentFileNames().size()) {
			return;
		}

		if (yPos0 == 4) {
			yOffset0++;
			auto newDirectoryName = lDisk->getParentFileNames()[4 + yOffset0];
			if (lDisk->moveBack()) {
				lDisk->moveForward(newDirectoryName);
				lDisk->initFiles();
				diskGui->setFileLoad(0);
				yOffset1 = 0;
				setChanged();
				notifyObservers(string("disk"));
			}
			return;
		}
		auto newDirectoryName = lDisk->getParentFileNames()[yPos0 + 1 + yOffset0];

		if (lDisk->moveBack()) {
			lDisk->initFiles();
			lDisk->moveForward(newDirectoryName);
			lDisk->initFiles();
			yPos0++;
			yOffset1 = 0;
			diskGui->setFileLoad(0);
			setChanged();
			notifyObservers(string("disk"));
			setChanged();
			notifyObservers(string("focus"));
		}
		return;
	}
	else {
		if (diskGui->getFileLoad() + 1 == lDisk->getFileNames().size())
			return;

		if (lDisk->getFileNames().size() == 0)
			return;

		auto yPos = diskGui->getFileLoad() - yOffset1;
		if (yPos == 4) {
			yOffset1++;
			diskGui->setFileLoad(diskGui->getFileLoad() + 1);
			setChanged();
			notifyObservers(string("right"));
			return;
		}
		diskGui->setFileLoad(diskGui->getFileLoad() + 1);
		setChanged();
		notifyObservers(string("focus"));
	}
}

mpc::disk::MpcFile* DirectoryGui::getSelectedFile()
{
	auto yPos = yPos0;
	if (xPos == 1)
		yPos = diskGui->getFileLoad() - yOffset1;

	return getFileFromGrid(xPos, yPos);
}

mpc::disk::MpcFile* DirectoryGui::getFileFromGrid(int x, int y)
{
	auto lDisk = mpc->getDisk().lock();
	mpc::disk::MpcFile* f = nullptr;
	if (x == 0 && lDisk->getParentFileNames().size() > y + yOffset0)
		f = lDisk->getParentFile(y + yOffset0);

	if (x == 1 && lDisk->getFileNames().size() > y + yOffset1)
		f = lDisk->getFile(y + yOffset1);

	return f;
}

void DirectoryGui::displayLeftFields(vector<weak_ptr<mpc::lcdgui::Field>> tfa)
{
	auto lDisk = mpc->getDisk().lock();
	int size = lDisk->getParentFileNames().size();
	for (int i = 0; i < 5; i++) {
		if (i + yOffset0 > size - 1) {
			tfa[i].lock()->setText("");
		}
		else {
			tfa[i].lock()->setText(lDisk->getParentFileNames()[i + yOffset0]);
		}
	}

	if (lDisk->isRoot())
		tfa[0].lock()->setText("ROOT");
}

void DirectoryGui::displayRightFields(vector<weak_ptr<mpc::lcdgui::Field>> tfa)
{
	auto lDisk = mpc->getDisk().lock();
	int size = lDisk->getFileNames().size();
	for (int i = 0; i < 5; i++) {
		if (i + yOffset1 > size - 1) {
			tfa[i].lock()->setText("");
		}
		else {
			string fileName = lDisk->getFileName(i + yOffset1);
			string name = moduru::lang::StrUtil::padRight(moduru::file::FileUtil::splitName(fileName)[0], " ", 16);
			string ext = moduru::file::FileUtil::splitName(fileName)[1];
			if (ext.length() > 0)
				ext = "." + ext;

			tfa[i].lock()->setText(name + ext);
		}
	}
}

void DirectoryGui::refreshFocus(vector<weak_ptr<mpc::lcdgui::Field>> tfa0, vector<weak_ptr<mpc::lcdgui::Field>> tfa1)
{
	auto ls = mpc->getLayeredScreen().lock();
	if (xPos == 0) {
		ls->setFocus(tfa0[yPos0].lock()->getName());
	}
	else if (xPos == 1) {
		ls->setFocus(tfa1[diskGui->getFileLoad() - yOffset1].lock()->getName());
	}
}

int DirectoryGui::getYOffsetFirst()
{
    return yOffset0;
}

int DirectoryGui::getYOffsetSecond()
{
    return yOffset1;
}

vector<string> DirectoryGui::getFirstColumn()
{
	auto lDisk = mpc->getDisk().lock();
	return lDisk->getParentFileNames();
}

vector<string> DirectoryGui::getSecondColumn()
{
	auto lDisk = mpc->getDisk().lock();
	return lDisk->getFileNames();
}

int DirectoryGui::getXPos()
{
    return xPos;
}

int DirectoryGui::getYpos0()
{
    return yPos0;
}

void DirectoryGui::setPreviousScreenName(string s)
{
    previousScreenName = s;
}

string DirectoryGui::getPreviousScreenName()
{
    return previousScreenName;
}

void DirectoryGui::findYOffset0()
{
	auto lDisk = mpc->getDisk().lock();
	auto names = lDisk->getParentFileNames();
	auto dirName = lDisk->getDirectoryName();
	auto size = names.size();
	for (int i = 0; i < size; i++) {
		if (names[i].compare(dirName) == 0) {
			yOffset0 = i;
			yPos0 = 0;
			break;
		}
	}
}

void DirectoryGui::setYOffset0(int i)
{
    yOffset0 = i;
}

void DirectoryGui::setYOffset1(int i)
{
	if (i < 0) return;
	yOffset1 = i;
}

void DirectoryGui::setYPos0(int i)
{
	yPos0 = i;
}

DirectoryGui::~DirectoryGui() {
}
