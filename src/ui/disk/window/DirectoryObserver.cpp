#include "DirectoryObserver.hpp"

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/disk/window/DirectoryGui.hpp>

#include <lang/StrUtil.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::ui::disk::window;
using namespace std;

DirectoryObserver::DirectoryObserver(weak_ptr<mpc::disk::AbstractDisk> disk, mpc::Mpc* mpc)
{
	this->mpc = mpc;
	this->disk = disk;
	auto lDisk = disk.lock();
	lDisk->addObserver(this);
	directoryGui = mpc->getUis().lock()->getDirectoryGui();
	directoryGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	a0Field = ls->lookupField("a0");
	a1Field = ls->lookupField("a1");
	a2Field = ls->lookupField("a2");
	a3Field = ls->lookupField("a3");
	a4Field = ls->lookupField("a4");
	b0Field = ls->lookupField("b0");
	b1Field = ls->lookupField("b1");
	b2Field = ls->lookupField("b2");
	b3Field = ls->lookupField("b3");
	b4Field = ls->lookupField("b4");
	left = { a0Field, a1Field, a2Field, a3Field, a4Field };
	right = { b0Field, b1Field, b2Field, b3Field, b4Field };
	topLeftLabel = ls->lookupLabel("topleft");
	topLeftLabel.lock()->setNoLeftMargin(true);
	topLeftLabel.lock()->enableRigorousClearing();
	a0Label = ls->lookupLabel("a0i");
	a1Label = ls->lookupLabel("a1i");
	a2Label = ls->lookupLabel("a2i");
	a3Label = ls->lookupLabel("a3i");
	a4Label = ls->lookupLabel("a4i");
	b0Label = ls->lookupLabel("b0i");
	b1Label = ls->lookupLabel("b1i");
	b2Label = ls->lookupLabel("b2i");
	b3Label = ls->lookupLabel("b3i");
	b4Label = ls->lookupLabel("b4i");
	c0Label = ls->lookupLabel("c0i");
	c1Label = ls->lookupLabel("c1i");
	c2Label = ls->lookupLabel("c2i");
	c3Label = ls->lookupLabel("c3i");
	c4Label = ls->lookupLabel("c4i");

	a0Label.lock()->enableRigorousClearing();
	a1Label.lock()->enableRigorousClearing();
	a2Label.lock()->enableRigorousClearing();
	a3Label.lock()->enableRigorousClearing();
	a4Label.lock()->enableRigorousClearing();
	b0Label.lock()->enableRigorousClearing();
	b1Label.lock()->enableRigorousClearing();
	b2Label.lock()->enableRigorousClearing();
	b3Label.lock()->enableRigorousClearing();
	b4Label.lock()->enableRigorousClearing();
	c0Label.lock()->enableRigorousClearing();
	c1Label.lock()->enableRigorousClearing();
	c2Label.lock()->enableRigorousClearing();
	c3Label.lock()->enableRigorousClearing();
	c4Label.lock()->enableRigorousClearing();


	updateLeft();
	updateRight();
	drawGraphicsLeft();
	drawGraphicsRight();
	updateFocus();
}

void DirectoryObserver::updateLeft()
{
	left = { a0Field, a1Field, a2Field, a3Field, a4Field };
	directoryGui->displayLeftFields(left);
}

void DirectoryObserver::updateRight()
{
	right = { b0Field, b1Field, b2Field, b3Field, b4Field };
	directoryGui->displayRightFields(right);
}

void DirectoryObserver::update(moduru::observer::Observable* o, std::any a)
{
	string param = std::any_cast<string>(a);

	if (param.compare("disk") == 0) {
		updateLeft();
		updateRight();
		drawGraphicsLeft();
		drawGraphicsRight();
	}
	else if (param.compare("right") == 0) {
		updateRight();
		drawGraphicsRight();
	}
	else if (param.compare("left") == 0) {
		updateLeft();
		drawGraphicsLeft();
		drawGraphicsRight();
	}
	else if (param.compare("focus") == 0) {
		updateFocus();
	}
}

void DirectoryObserver::updateFocus()
{
	directoryGui->refreshFocus(left, right);
}

void DirectoryObserver::drawGraphicsLeft()
{
	auto topLeft = topLeftLabel.lock();
	auto a0 = a0Label.lock();
	auto a1 = a1Label.lock();
	auto a2 = a2Label.lock();
	auto a3 = a3Label.lock();
	auto a4 = a4Label.lock();
	topLeft->setText("");
	a0->setText("");
	a1->setText("");
	a2->setText("");
	a3->setText("");
	a4->setText("");
	auto fc = directoryGui->getFirstColumn();
	auto currentDirIcons = vector<string>{ u8"\u00EF", u8"\u00F1", u8"\u00F0" };
	auto dirIcons = vector<string>{ u8"\u00EA", u8"\u00EB", u8"\u00EC" };
	string notRootDash = u8"\u00ED";
	string onlyDirIcon = u8"\u00F3";
	string rootIcon = u8"\u00EE";
	int size = fc.size();
	if (size == 0) {
		a0->setText(rootIcon);
		return;
	}
	topLeft->setText(notRootDash);
	int offset = directoryGui->getYOffsetFirst();
	if (size - offset == 1) {
		if (size > 1) {
			a0->setText(currentDirIcons[2]);
		}
		else {
			a0->setText(onlyDirIcon);
		}
		return;
	}
	int lastVisibleFileNumber = size - offset - 1;
	auto firstVisibleFile = directoryGui->getFileFromGrid(0, 0);
	mpc::disk::MpcFile* lastVisibleFile = nullptr;
	if (lastVisibleFileNumber > 0) {
		if (lastVisibleFileNumber > 4)
			lastVisibleFileNumber = 4;

		lastVisibleFile = directoryGui->getFileFromGrid(0, lastVisibleFileNumber);
	}
	auto lDisk = disk.lock();
	auto dirName = lDisk->getDirectoryName();
	int visibleListLength = lastVisibleFileNumber + 1;
	if (size - offset == 2) {
		if (firstVisibleFile->getName().compare(dirName) == 0) {
			a0->setText(currentDirIcons[0]);
		}
		else {
			a0->setText(dirIcons[0]);
		}
		if (lastVisibleFile->getName().compare(dirName) == 0) {
			a1->setText(currentDirIcons[2]);
		}
		else {
			a1->setText(dirIcons[2]);
		}
		return;
	}
	auto aLabels = vector < shared_ptr<mpc::lcdgui::Label>>{ a0, a1, a2, a3, a4 };
	if (size - offset <= 4) {
		if (firstVisibleFile->getName().compare(dirName) == 0) {
			a0->setText(currentDirIcons[0]);
		}
		else {
			a0->setText(dirIcons[0]);
		}
		for (int i = 1; i < visibleListLength - 1; i++) {
			if (fc[i].compare(dirName) == 0) {
				aLabels[i]->setText(currentDirIcons[1]);
			}
			else {
				aLabels[i]->setText(dirIcons[1]);
			}
		}
		if (lastVisibleFile->getName().compare(dirName) == 0) {
			aLabels[visibleListLength - 1]->setText(currentDirIcons[2]);
		}
		else {
			aLabels[visibleListLength - 1]->setText(dirIcons[2]);
		}
		return;
	}
	if (size - offset >= 5) {
		if (firstVisibleFile->getName().compare(dirName) == 0) {
			if (firstVisibleFile->getName().compare(fc[0]) == 0) {
				a0->setText(currentDirIcons[0]);
			}
			else {
				a0->setText(currentDirIcons[1]);
			}
		}
		else {
			if (firstVisibleFile->getName().compare(fc[0]) == 0) {
				a0->setText(dirIcons[0]);
			}
			else {
				a0->setText(dirIcons[1]);
			}
		}
		for (int i = 1; i < visibleListLength - 1; i++) {
			if (fc[i + offset].compare(dirName) == 0) {
				aLabels[i]->setText(currentDirIcons[1]);
			}
			else {
				aLabels[i]->setText(dirIcons[1]);
			}
			if (i == 3)
				break;
		}
		if (lastVisibleFile->getName().compare(dirName) == 0) {
			if (lastVisibleFile->getName().compare(fc[(int)(size)-1]) == 0) {
				a4->setText(currentDirIcons[2]);
			}
			else {
				a4->setText(currentDirIcons[1]);
			}
		}
		else {
			if (lastVisibleFile->getName().compare(fc[(int)(size)-1]) == 0) {
				a4->setText(dirIcons[2]);
			}
			else {
				a4->setText(dirIcons[1]);
			}
		}
		return;
	}
}

void DirectoryObserver::drawGraphicsRight()
{
	auto b0 = b0Label.lock();
	auto b1 = b1Label.lock();
	auto b2 = b2Label.lock();
	auto b3 = b3Label.lock();
	auto b4 = b4Label.lock();
	b0->setText("");
	b1->setText("");
	b2->setText("");
	b3->setText("");
	b4->setText("");
	auto lDisk = disk.lock();
	auto firstColumn = directoryGui->getFirstColumn();
	auto secondColumn = directoryGui->getSecondColumn();
	auto parentNames = lDisk->getParentFileNames();
	auto dirName = lDisk->getDirectoryName();
	int yOffset1 = directoryGui->getYOffsetFirst();
	int yOffset2 = directoryGui->getYOffsetSecond();

	auto a0 = a0Field.lock();
	auto a1 = a1Field.lock();
	auto a2 = a2Field.lock();
	auto a3 = a3Field.lock();
	auto a4 = a4Field.lock();

	if (parentNames.size() == 0) {
		a0Field.lock()->setText(padFileName(a0->getText(), u8"\u00DF"));
		b0->setText(u8"\u00E0");
	}

	int size = firstColumn.size();

	if (size - yOffset1 > 0 && directoryGui->getFileFromGrid(0, 0)->getName().compare(dirName) == 0) {
		b0->setText(u8"\u00E0");
		a0Field.lock()->setText(padFileName(a0->getText(), u8"\u00DF"));
	}
	if (size - yOffset1 > 1 && directoryGui->getFileFromGrid(0, 1)->getName().compare(dirName) == 0) {
		b1->setText(u8"\u00E0");
		a1Field.lock()->setText(padFileName(a1->getText(), u8"\u00DF"));
	}
	if (size - yOffset1 > 2 && directoryGui->getFileFromGrid(0, 2)->getName().compare(dirName) == 0) {
		b2->setText(u8"\u00E0");
		a2Field.lock()->setText(padFileName(a2->getText(), u8"\u00DF"));
	}
	if (size - yOffset1 > 3 && directoryGui->getFileFromGrid(0, 3)->getName().compare(dirName) == 0) {
		b3->setText(u8"\u00E0");
		a3Field.lock()->setText(padFileName(a3->getText(), u8"\u00DF"));
	}
	if (size - yOffset1 > 4 && directoryGui->getFileFromGrid(0, 4)->getName().compare(dirName) == 0) {
		b4->setText(u8"\u00E0");
		a4Field.lock()->setText(padFileName(a4->getText(), u8"\u00DF"));
	}

	auto c0 = c0Label.lock();
	auto c1 = c1Label.lock();
	auto c2 = c2Label.lock();
	auto c3 = c3Label.lock();
	auto c4 = c4Label.lock();

	if (directoryGui->getFileFromGrid(1, 0) != nullptr && directoryGui->getFileFromGrid(1, 0)->isDirectory()) {
		if (yOffset2 == 0) {
			c0->setText(u8"\u00E1");
		}
		else {
			c0->setText(u8"\u00E6");
		}
	}
	else {
		if (yOffset2 == 0) {
			c0->setText(u8"\u00E5");
		}
		else {
			c0->setText(u8"\u00E3");
		}
	}

	if (directoryGui->getFileFromGrid(1, 1) != nullptr && directoryGui->getFileFromGrid(1, 1)->isDirectory()) {
		c1->setText(u8"\u00E6");
	}
	else {
		c1->setText(u8"\u00E3");
	}
	if (directoryGui->getFileFromGrid(1, 2) != nullptr && directoryGui->getFileFromGrid(1, 2)->isDirectory()) {
		c2->setText(u8"\u00E6");
	}
	else {
		c2->setText(u8"\u00E3");
	}
	if (directoryGui->getFileFromGrid(1, 3) != nullptr && directoryGui->getFileFromGrid(1, 3)->isDirectory()) {
		c3->setText(u8"\u00E6");
	}
	else {
		c3->setText(u8"\u00E3");
	}
	if (directoryGui->getFileFromGrid(1, 4) != nullptr && directoryGui->getFileFromGrid(1, 4)->isDirectory()) {
		if (yOffset2 + 5 == secondColumn.size()) {
			c4->setText(u8"\u00E2");
		}
		else {
			c4->setText(u8"\u00E6");
		}
	}
	else {
		if (yOffset2 + 5 == secondColumn.size() || secondColumn.size() <= 5) {
			c4->setText(u8"\u00E4");
		}
		else {
			c4->setText(u8"\u00E3");
		}
	}
}

string DirectoryObserver::padFileName(string s, string pad) {
	string res = moduru::lang::StrUtil::padRight(moduru::lang::StrUtil::trim(s), pad, 8);
	return res;
}

DirectoryObserver::~DirectoryObserver() {
	auto lDisk = disk.lock();
	lDisk->deleteObserver(this);
	directoryGui->deleteObserver(this);
}
