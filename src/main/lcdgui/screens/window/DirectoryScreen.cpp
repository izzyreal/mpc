#include "DirectoryScreen.hpp"

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>

#include <Util.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace moduru::file;
using namespace std;

DirectoryScreen::DirectoryScreen(const int layerIndex)
	: ScreenComponent("directory", layerIndex)
{
}

void DirectoryScreen::open()
{
	displayLeftFields();
	displayRightFields();
	drawGraphicsLeft();
	drawGraphicsRight();
	refreshFocus();
}

void DirectoryScreen::function(int f)
{
	BaseControls::function(f);
	
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
	auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));

	switch (f)
	{
	case 1:
		if (loadScreen->getSelectedFile() == nullptr) return;

		if (loadScreen->getSelectedFile()->isDirectory()) {
			ls.lock()->openScreen("delete-folder");
		}
		else {
			ls.lock()->openScreen("delete-file");
		}
		break;
	case 2:
	{
		if (loadScreen->getSelectedFile() == nullptr)
		{
			return;
		}

		auto fileNameNoExt = mpc::Util::splitName(loadScreen->getSelectedFile()->getName())[0];
		nameScreen->setName(fileNameNoExt);
		
		if (loadScreen->getSelectedFile()->isDirectory())
		{
			nameScreen->setNameLimit(8);
		}
		
		nameScreen->parameterName = "rename";
		ls.lock()->openScreen("name");
		break;
	}
	case 4:
		if (xPos == 0)
		{
			return;
		}

		nameScreen->setName("NEWFOLDR");
		nameScreen->setNameLimit(8);
		nameScreen->parameterName = "newfolder";
		ls.lock()->openScreen("name");
		break;
	case 5:
	{
		auto controls = Mpc::instance().getControls().lock();

		if (controls->isF6Pressed()) {
			return;
		}

		controls->setF6Pressed(true);

		auto file = loadScreen->getSelectedFile();

		if (!file->isDirectory()) {

			bool started = mpc.getAudioMidiServices().lock()->getSoundPlayer().lock()->start(file->getFile().lock()->getPath());

			auto name = file->getFsNode().lock()->getNameWithoutExtension();

			if (started) {
				Mpc::instance().getLayeredScreen().lock()->createPopup("Playing " + name);
			}
			else {
				Mpc::instance().getLayeredScreen().lock()->createPopup("Can't play " + name);
			}
		}

		break;
	}
	}
}

void DirectoryScreen::turnWheel(int i)
{	
	if (i > 0)
	{
		down();
	}
	else {
		up();
	}
}

void DirectoryScreen::left()
{
	auto disk = mpc.getDisk().lock();
	
	auto prevDirName = disk->getDirectoryName();

	if (xPos == 1)
	{
		xPos--;
		refreshFocus();
		return;
	}
	else {
		if (disk->moveBack())
		{
			auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
			disk->initFiles();
			
			loadScreen->fileLoad = 0;
			yPos0 = 0;
			
			for (int i = 0; i < disk->getParentFileNames().size(); i++)
			{
				if (disk->getParentFileNames()[i].compare(disk->getDirectoryName()) == 0)
				{
					yOffset0 = i;
					break;
				}
			}

			for (int i = 0; i < disk->getFileNames().size(); i++)
			{
				if (disk->getFileNames()[i].compare(prevDirName) == 0)
				{
					yOffset1 = i;
					loadScreen->fileLoad = i;
					break;
				}
			}

			if (yOffset1 + 1 > disk->getFileNames().size())
			{
				yOffset1 = 0;
			}

			if (disk->getParentFileNames().size() == 0)
			{
				yOffset0 = 0;
			}

			displayLeftFields();
			displayRightFields();
			drawGraphicsLeft();
			drawGraphicsRight();

			refreshFocus();
		}
	}
}

void DirectoryScreen::right()
{
	if (xPos == 0)
	{
		xPos++;
		refreshFocus();
	}
	else
	{
		auto disk = mpc.getDisk().lock();
		
		if (getSelectedFile() == nullptr || disk->getFileNames().size() == 0 || !getSelectedFile()->isDirectory())
		{
			return;
		}

		auto f = getSelectedFile();
		
		if (f == nullptr)
		{
			return;
		}

		if (!disk->moveForward(f->getName()))
		{
			return;
		}

		disk->initFiles();
		
		yPos0 = 0;
		yOffset1 = 0;
		
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
		loadScreen->fileLoad = 0;

		for (int i = 0; i < disk->getParentFileNames().size(); i++)
		{
			if (disk->getParentFileNames()[i].compare(f->getName()) == 0)
			{
				yOffset0 = i;
				break;
			}
		}

		displayLeftFields();
		displayRightFields();
		drawGraphicsLeft();
		drawGraphicsRight();

		refreshFocus();
	}
}

void DirectoryScreen::up()
{
	auto disk = mpc.getDisk().lock();
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

	if (xPos == 0)
	{
		if (yOffset0 == 0 && yPos0 == 0)
		{
			return;
		}

		if (yPos0 == 0)
		{
			yOffset0--;
		
			auto newDirectoryName = disk->getParentFileNames()[yOffset0];
			
			if (disk->moveBack())
			{
				disk->initFiles();
				disk->moveForward(newDirectoryName);
				disk->initFiles();

				loadScreen->fileLoad = 0;

				yOffset1 = 0;

				displayLeftFields();
				displayRightFields();
				drawGraphicsLeft();
				drawGraphicsRight();
			}
			return;
		}

		auto newDirectoryName = disk->getParentFileNames()[yPos0 - 1 + yOffset0];
		
		if (disk->moveBack())
		{
			disk->initFiles();
			disk->moveForward(newDirectoryName);
			disk->initFiles();

			yPos0--;
			yOffset1 = 0;

			loadScreen->fileLoad = 0;

			displayLeftFields();
			displayRightFields();
			drawGraphicsLeft();
			drawGraphicsRight();
			refreshFocus();
		}
	}
	else
	{
		if (loadScreen->fileLoad == 0)
		{
			return;
		}

		auto yPos = loadScreen->fileLoad - yOffset1;
		
		if (yPos == 0)
		{
			yOffset1--;
			loadScreen->fileLoad -= 1;
			displayLeftFields();
			displayRightFields();
			drawGraphicsRight();
		}
		else
		{
			loadScreen->fileLoad -= 1;
			refreshFocus();
		}
	}
}

void DirectoryScreen::down()
{
	auto disk = mpc.getDisk().lock();
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

	if (xPos == 0)
	{
		if (disk->isRoot())
		{
			return;
		}

		if (yOffset0 + yPos0 + 1 >= disk->getParentFileNames().size())
		{
			return;
		}

		if (yPos0 == 4)
		{
			yOffset0++;
			auto newDirectoryName = disk->getParentFileNames()[4 + yOffset0];
		
			if (disk->moveBack())
			{
				disk->initFiles();
				disk->moveForward(newDirectoryName);
				disk->initFiles();
				loadScreen->fileLoad = 0;
				yOffset1 = 0;

				displayLeftFields();
				displayRightFields();
				drawGraphicsLeft();
				drawGraphicsRight();
			}
			return;
		}

		auto newDirectoryName = disk->getParentFileNames()[yPos0 + 1 + yOffset0];

		if (disk->moveBack())
		{
			disk->initFiles();
			disk->moveForward(newDirectoryName);
			disk->initFiles();

			yPos0++;
			yOffset1 = 0;
			loadScreen->fileLoad = 0;

			displayLeftFields();
			displayRightFields();
			drawGraphicsLeft();
			drawGraphicsRight();

			refreshFocus();
		}
		return;
	}
	else
	{
		if (loadScreen->fileLoad + 1 == disk->getFileNames().size())
		{
			return;
		}

		if (disk->getFileNames().size() == 0)
		{
			return;
		}

		auto yPos = loadScreen->fileLoad - yOffset1;

		if (yPos == 4)
		{
			yOffset1++;
			loadScreen->fileLoad += 1;
			displayLeftFields();
			displayRightFields();
			drawGraphicsRight();
		}
		else {
			loadScreen->fileLoad += 1;
			refreshFocus();
		}
	}
}

mpc::disk::MpcFile* DirectoryScreen::getSelectedFile()
{
	auto yPos = yPos0;

	if (xPos == 1)
	{
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
		yPos = loadScreen->fileLoad - yOffset1;
	}

	return getFileFromGrid(xPos, yPos);
}

mpc::disk::MpcFile* DirectoryScreen::getFileFromGrid(int x, int y)
{
	auto disk = mpc.getDisk().lock();
	mpc::disk::MpcFile* f = nullptr;
	
	if (x == 0 && disk->getParentFileNames().size() > y + yOffset0)
	{
		f = disk->getParentFile(y + yOffset0);
	}

	if (x == 1 && disk->getFileNames().size() > y + yOffset1)
	{
		f = disk->getFile(y + yOffset1);
	}

	return f;
}

void DirectoryScreen::displayLeftFields()
{
	auto disk = mpc.getDisk().lock();

	auto names = disk->getParentFileNames();

	int size = names.size();

	for (int i = 0; i < 5; i++)
	{
		if (i + yOffset0 > size - 1)
		{
			findField("a" + to_string(i)).lock()->setText(" ");
		}
		else
		{
			findField("a" + to_string(i)).lock()->setText(names[i + yOffset0]);
		}
	}

	if (disk->isRoot())
	{
		findField("a0").lock()->setText("ROOT");
	}
}

void DirectoryScreen::displayRightFields()
{
	auto disk = mpc.getDisk().lock();
	int size = disk->getFileNames().size();

	for (int i = 0; i < 5; i++)
	{
		if (i + yOffset1 > size - 1)
		{
			findField("b" + to_string(i)).lock()->setText(" ");
		}
		else
		{
			string fileName = disk->getFileName(i + yOffset1);
			string name = StrUtil::padRight(FileUtil::splitName(fileName)[0], " ", 16);
			string ext = FileUtil::splitName(fileName)[1];

			if (ext.length() > 0)
			{
				ext = "." + ext;
			}

			findField("b" + to_string(i)).lock()->setText(name + ext);
		}
	}
}

void DirectoryScreen::refreshFocus()
{
	if (xPos == 0)
	{
		ls.lock()->setFocus("a" + to_string(yPos0));
	}
	else if (xPos == 1)
	{
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
		ls.lock()->setFocus("b" + to_string(loadScreen->fileLoad - yOffset1));
	}
}

vector<string> DirectoryScreen::getFirstColumn()
{
	return mpc.getDisk().lock()->getParentFileNames();
}

vector<string> DirectoryScreen::getSecondColumn()
{
	return mpc.getDisk().lock()->getFileNames();
}

int DirectoryScreen::getXPos()
{
	return xPos;
}

int DirectoryScreen::getYpos0()
{
	return yPos0;
}

void DirectoryScreen::findYOffset0()
{
	auto disk = mpc.getDisk().lock();
	auto names = disk->getParentFileNames();
	auto dirName = disk->getDirectoryName();
	auto size = names.size();
	
	for (int i = 0; i < size; i++)
	{
		if (names[i].compare(dirName) == 0)
		{
			yOffset0 = i;
			yPos0 = 0;
			break;
		}
	}
}

void DirectoryScreen::setYOffset0(int i)
{
	yOffset0 = i;
}

void DirectoryScreen::setYOffset1(int i)
{
	if (i < 0)
	{
		return;
	}
	yOffset1 = i;
}

void DirectoryScreen::setYPos0(int i)
{
	yPos0 = i;
}

void DirectoryScreen::drawGraphicsLeft()
{
	auto topLeft = findLabel("topleft").lock();
	auto a0 = findLabel("a0i").lock();
	auto a1 = findLabel("a1i").lock();
	auto a2 = findLabel("a2i").lock();
	auto a3 = findLabel("a3i").lock();
	auto a4 = findLabel("a4i").lock();
	topLeft->setText(" ");
	a0->setText(" ");
	a1->setText(" ");
	a2->setText(" ");
	a3->setText(" ");
	a4->setText(" ");

	auto fc = getFirstColumn();
	auto currentDirIcons = vector<string>{ u8"\u00EF", u8"\u00F1", u8"\u00F0" };
	auto dirIcons = vector<string>{ u8"\u00EA", u8"\u00EB", u8"\u00EC" };
	
	string notRootDash = u8"\u00ED";
	string onlyDirIcon = u8"\u00F3";
	string rootIcon = u8"\u00EE";
	
	int size = fc.size();
	
	if (size == 0)
	{
		a0->setText(rootIcon);
		return;
	}

	topLeft->setText(notRootDash);

	if (size - yOffset0 == 1)
	{
		if (size > 1)
		{
			a0->setText(currentDirIcons[2]);
		}
		else
		{
			a0->setText(onlyDirIcon);
		}
		return;
	}
	
	int lastVisibleFileNumber = size - yOffset0 - 1;
	auto firstVisibleFile = getFileFromGrid(0, 0);
	mpc::disk::MpcFile* lastVisibleFile = nullptr;
	
	if (lastVisibleFileNumber > 0)
	{
		if (lastVisibleFileNumber > 4)
		{
			lastVisibleFileNumber = 4;
		}

		lastVisibleFile = getFileFromGrid(0, lastVisibleFileNumber);
	}

	auto disk = mpc.getDisk().lock();
	auto dirName = disk->getDirectoryName();
	int visibleListLength = lastVisibleFileNumber + 1;

	if ( (size - yOffset0) == 2)
	{
		if (firstVisibleFile->getName().compare(dirName) == 0)
		{
			a0->setText(currentDirIcons[0]);
		}
		else
		{
			a0->setText(dirIcons[0]);
		}
		
		if (lastVisibleFile->getName().compare(dirName) == 0)
		{
			a1->setText(currentDirIcons[2]);
		}
		else
		{
			a1->setText(dirIcons[2]);
		}

		return;
	}

	auto aLabels = vector < shared_ptr<mpc::lcdgui::Label>>{ a0, a1, a2, a3, a4 };
	
	if (size - yOffset0 <= 4)
	{
		if (firstVisibleFile->getName().compare(dirName) == 0)
		{
			a0->setText(currentDirIcons[0]);
		}
		else
		{
			a0->setText(dirIcons[0]);
		}
		
		for (int i = 1; i < visibleListLength - 1; i++)
		{
			if (fc[i].compare(dirName) == 0)
			{
				aLabels[i]->setText(currentDirIcons[1]);
			}
			else
			{
				aLabels[i]->setText(dirIcons[1]);
			}
		}

		if (lastVisibleFile->getName().compare(dirName) == 0)
		{
			aLabels[visibleListLength - 1]->setText(currentDirIcons[2]);
		}
		else
		{
			aLabels[visibleListLength - 1]->setText(dirIcons[2]);
		}

		return;
	}
	
	if (size - yOffset0 >= 5)
	{
		if (firstVisibleFile->getName().compare(dirName) == 0)
		{
			if (firstVisibleFile->getName().compare(fc[0]) == 0)
			{
				a0->setText(currentDirIcons[0]);
			}
			else
			{
				a0->setText(currentDirIcons[1]);
			}
		}
		else {
			if (firstVisibleFile->getName().compare(fc[0]) == 0)
			{
				a0->setText(dirIcons[0]);
			}
			else {
				a0->setText(dirIcons[1]);
			}
		}
		
		for (int i = 1; i < visibleListLength - 1; i++)
		{
			if (fc[i + yOffset0].compare(dirName) == 0)
			{
				aLabels[i]->setText(currentDirIcons[1]);
			}
			else
			{
				aLabels[i]->setText(dirIcons[1]);
			}

			if (i == 3)
			{
				break;
			}
		}

		if (lastVisibleFile->getName().compare(dirName) == 0)
		{
			if (lastVisibleFile->getName().compare(fc[(int)(size)-1]) == 0)
			{
				a4->setText(currentDirIcons[2]);
			}
			else
			{
				a4->setText(currentDirIcons[1]);
			}
		}
		else
		{
			if (lastVisibleFile->getName().compare(fc[(int)(size)-1]) == 0)
			{
				a4->setText(dirIcons[2]);
			}
			else {
				a4->setText(dirIcons[1]);
			}
		}
	}
}

void DirectoryScreen::drawGraphicsRight()
{
	auto b0 = findLabel("b0i").lock();
	auto b1 = findLabel("b1i").lock();
	auto b2 = findLabel("b2i").lock();
	auto b3 = findLabel("b3i").lock();
	auto b4 = findLabel("b4i").lock();

	b0->setText(" ");
	b1->setText(" ");
	b2->setText(" ");
	b3->setText(" ");
	b4->setText(" ");

	auto firstColumn = getFirstColumn();
	auto secondColumn = getSecondColumn();

	auto disk = mpc.getDisk().lock();

	auto parentNames = disk->getParentFileNames();
	auto dirName = disk->getDirectoryName();

	auto a0 = findField("a0").lock();
	auto a1 = findField("a1").lock();
	auto a2 = findField("a2").lock();
	auto a3 = findField("a3").lock();
	auto a4 = findField("a4").lock();

	if (parentNames.size() == 0)
	{
		a0->setText(padFileName(a0->getText(), u8"\u00DF"));
		b0->setText(u8"\u00E0");
	}

	int size = firstColumn.size();

	auto file00 = getFileFromGrid(0, 0);
	auto file01 = getFileFromGrid(0, 1);
	auto file02 = getFileFromGrid(0, 2);
	auto file03 = getFileFromGrid(0, 3);
	auto file04 = getFileFromGrid(0, 4);

	if (size - yOffset0 > 0 && file00->getName().compare(dirName) == 0)
	{
		a0->setText(padFileName(a0->getText(), u8"\u00DF"));
		b0->setText(u8"\u00E0");
	}

	if (size - yOffset0 > 1 && file01->getName().compare(dirName) == 0)
	{
		a1->setText(padFileName(a1->getText(), u8"\u00DF"));
		b1->setText(u8"\u00E0");
	}

	if (size - yOffset0 > 2 && getFileFromGrid(0, 2)->getName().compare(dirName) == 0)
	{
		a2->setText(padFileName(a2->getText(), u8"\u00DF"));
		b2->setText(u8"\u00E0");
	}

	if (size - yOffset0 > 3 && file03->getName().compare(dirName) == 0)
	{
		a3->setText(padFileName(a3->getText(), u8"\u00DF"));
		b3->setText(u8"\u00E0");
	}

	if (size - yOffset0 > 4 && file04->getName().compare(dirName) == 0)
	{
		a4->setText(padFileName(a4->getText(), u8"\u00DF"));
		b4->setText(u8"\u00E0");
	}

	auto c0 = findLabel("c0i").lock();
	auto c1 = findLabel("c1i").lock();
	auto c2 = findLabel("c2i").lock();
	auto c3 = findLabel("c3i").lock();
	auto c4 = findLabel("c4i").lock();

	auto file10 = getFileFromGrid(1, 0);

	if (file10 != nullptr && file10->isDirectory())
	{
		if (yOffset1 == 0)
		{
			// Set first folder icon 
			c0->setText(u8"\u00E1");
		}
		else {
			// Set not first folder icon
			c0->setText(u8"\u00E6");
		}
	}
	else
	{
		if (yOffset1 == 0)
		{
			// Set first file icon
			c0->setText(u8"\u00E5");
		}
		else
		{
			// Set not first file icon
			c0->setText(u8"\u00E3");
		}
	}

	auto file11 = getFileFromGrid(1, 1);

	if (file11 != nullptr && file11->isDirectory())
	{
		c1->setText(u8"\u00E6");
	}
	else
	{
		c1->setText(u8"\u00E3");
	}

	auto file12 = getFileFromGrid(1, 2);

	if (file12 != nullptr && file12->isDirectory())
	{
		c2->setText(u8"\u00E6");
	}
	else
	{
		c2->setText(u8"\u00E3");
	}

	auto file13 = getFileFromGrid(1, 3);

	if (file13 != nullptr && file13->isDirectory())
	{
		c3->setText(u8"\u00E6");
	}
	else
	{
		c3->setText(u8"\u00E3");
	}

	auto file14 = getFileFromGrid(1, 4);

	if (file14 != nullptr && file14->isDirectory())
	{
		if (yOffset1 + 5 == secondColumn.size())
		{
			c4->setText(u8"\u00E2");
		}
		else
		{
			c4->setText(u8"\u00E6");
		}
	}
	else
	{
		if (yOffset1 + 5 == secondColumn.size() || secondColumn.size() <= 5)
		{
			c4->setText(u8"\u00E4");
		}
		else
		{
			c4->setText(u8"\u00E3");
		}
	}
}

string DirectoryScreen::padFileName(string s, string pad)
{
	return StrUtil::padRight(moduru::lang::StrUtil::trim(s), pad, 8);
}
