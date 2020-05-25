#include "SaveASoundControls.hpp"

#include <disk/AbstractDisk.hpp>
#include <Util.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::disk::window;
using namespace std;

SaveASoundControls::SaveASoundControls() 
	: AbstractDiskControls()
{
}

void SaveASoundControls::turnWheel(int i)
{
	init();
	
	if (param.compare("file") == 0 && i > 0) {
		sampler.lock()->selectPreviousSound();
	}
	else if (param.compare("file") == 0 && i < 0) {
		sampler.lock()->selectNextSound();
	}
	else if (param.compare("filetype") == 0) {
		diskGui->setFileTypeSaveSound(diskGui->getFileTypeSaveSound() + i);
	}
}

void SaveASoundControls::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
	{
		auto s = sampler.lock()->getSound().lock();
		auto type = diskGui->getFileTypeSaveSound();
		auto ext = string(type == 0 ? ".SND" : ".WAV");
		auto fileName = mpc::Util::getFileName(nameGui->getName()) + ext;

		if (disk.lock()->checkExists(fileName))
		{
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		
		disk.lock()->flush();
		disk.lock()->initFiles();
		auto f = disk.lock()->newFile(fileName);

		if (type == 0)
		{
			disk.lock()->writeSound(s.get(), f);
		}
		else
		{
			disk.lock()->writeWav(s.get(), f);
		}

		disk.lock()->flush();
		disk.lock()->initFiles();
		ls.lock()->openScreen("save");
		break;
	}
	}
}
