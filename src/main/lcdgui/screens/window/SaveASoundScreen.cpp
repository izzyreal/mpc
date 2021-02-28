#include "SaveASoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <disk/AbstractDisk.hpp>
#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

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
	
	if (param.compare("file") == 0 && i > 0)
	{
		sampler.lock()->selectPreviousSound();
	}
	else if (param.compare("file") == 0 && i < 0)
	{
		sampler.lock()->selectNextSound();
	}
	else if (param.compare("file-type") == 0)
	{
		setFileType(fileType + i);
	}
    
    if (param.compare("file") == 0)
    {
        auto saveName = sampler.lock()->getSound().lock()->getName();
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
		auto s = sampler.lock()->getSound().lock();
		auto ext = string(fileType == 0 ? ".SND" : ".WAV");
		auto fileName = mpc::Util::getFileName(mpc.screens->get<NameScreen>("name")->getName()) + ext;

		if (disk->checkExists(fileName))
		{
			openScreen("file-exists");
			return;
		}
		
		disk->flush();
		disk->initFiles();
		
		auto f = disk->newFile(fileName);

		if (fileType == 0)
			disk->writeSound(s, f);
		else
			disk->writeWav(s, f);

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
	findField("file-type").lock()->setText(string(fileType == 0 ? "MPC2000" : "WAV"));
}

void SaveASoundScreen::displayFile()
{
	findField("file").lock()->setText(mpc.screens->get<NameScreen>("name")->getName());
}
