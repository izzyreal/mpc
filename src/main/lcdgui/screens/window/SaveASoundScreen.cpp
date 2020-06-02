#include "SaveASoundScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SaveASoundScreen::SaveASoundScreen(const int layerIndex) 
	: ScreenComponent("save-a-sound", layerIndex)
{
}

void SaveASoundScreen::open()
{
	if (ls.lock()->getPreviousScreenName().compare("name") != 0)
	{
		saveName = sampler.lock()->getSound().lock()->getName();
	}

	displayFile();
	displayFileType();
}

void SaveASoundScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("file") == 0 && i > 0)
	{
		sampler.lock()->selectPreviousSound();
		saveName = sampler.lock()->getSound().lock()->getName();
		displayFile();
	}
	else if (param.compare("file") == 0 && i < 0)
	{
		sampler.lock()->selectNextSound();
		saveName = sampler.lock()->getSound().lock()->getName();
		displayFile();
	}
	else if (param.compare("filetype") == 0)
	{
		setFileType(fileType + i);
	}
}

void SaveASoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto disk = mpc.getDisk().lock();
		auto s = sampler.lock()->getSound().lock();
		auto ext = string(fileType == 0 ? ".SND" : ".WAV");
		auto fileName = mpc::Util::getFileName(saveName) + ext;

		if (disk->checkExists(fileName))
		{
			ls.lock()->openScreen("file-already-exists");
			return;
		}
		
		disk->flush();
		disk->initFiles();
		
		auto f = disk->newFile(fileName);

		if (fileType == 0)
		{
			disk->writeSound(s.get(), f);
		}
		else
		{
			disk->writeWav(s.get(), f);
		}

		disk->flush();
		disk->initFiles();
		ls.lock()->openScreen("save");
		break;
	}
	}
}

void SaveASoundScreen::setFileType(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}

	fileType = i;
	displayFileType();
}

void SaveASoundScreen::displayFileType()
{
	findField("file-type").lock()->setText(string(fileType == 0 ? "MPC2000" : "WAV"));
}

void SaveASoundScreen::displayFile()
{
	findField("file").lock()->setText(saveName);
}
