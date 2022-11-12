#include "SaveASoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <disk/AbstractDisk.hpp>
#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;

SaveASoundScreen::SaveASoundScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "save-a-sound", layerIndex)
{
}

void SaveASoundScreen::open()
{
	displayFile();
	displayFileType();
}

void SaveASoundScreen::turnWheel(int i)
{
	init();
	
	if (param == "file" && i > 0)
	{
		sampler->selectPreviousSound();
	}
	else if (param == "file" && i < 0)
	{
		sampler->selectNextSound();
	}
	else if (param == "file-type")
	{
		setFileType(fileType + i);
	}
    
    if (param == "file")
    {
        auto saveName = sampler->getSound().lock()->getName();
        mpc.screens->get<NameScreen>("name")->setName(saveName);
        displayFile();
    }
}

void SaveASoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("save");
		break;
	case 4:
	{
		auto disk = mpc.getDisk().lock();
		auto s = sampler->getSound().lock();
		auto ext = std::string(fileType == 0 ? ".SND" : ".WAV");
		auto fileName = mpc::Util::getFileName(mpc.screens->get<NameScreen>("name")->getNameWithoutSpaces()) + ext;

		if (disk->checkExists(fileName))
		{
			openScreen("file-exists");
			return;
		}
		
		disk->flush();
		disk->initFiles();
		
		if (fileType == 0)
			disk->writeSnd(s, fileName);
		else
			disk->writeWav(s, fileName);

		disk->flush();
		disk->initFiles();
		openScreen("save");
		break;
	}
	}
}

void SaveASoundScreen::setFileType(int i)
{
	if (i < 0 || i > 1)
		return;

	fileType = i;
	displayFileType();
}

void SaveASoundScreen::displayFileType()
{
	findField("file-type")->setText(std::string(fileType == 0 ? "MPC2000" : "WAV"));
}

void SaveASoundScreen::displayFile()
{
	findField("file")->setText(mpc.screens->get<NameScreen>("name")->getNameWithoutSpaces());
}
