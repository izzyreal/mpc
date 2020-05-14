#include "SaveASoundControls.hpp"

#include <disk/AbstractDisk.hpp>
#include <Util.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/sampler/SoundGui.hpp>
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
		sampler.lock()->setSoundGuiPrevSound();
	}
	else if (param.compare("file") == 0 && i < 0) {
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("filetype") == 0) {
		diskGui->setFileTypeSaveSound(diskGui->getFileTypeSaveSound() + i);
	}
}

void SaveASoundControls::function(int i)
{
	init();
	weak_ptr<mpc::sampler::Sound> s;
	int type;
	string ext;
	string fileName;
	mpc::disk::MpcFile* f;
	
	switch (i) {
	case 3:
		ls.lock()->openScreen("save");
		break;
	case 4:
		s = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock());
		type = diskGui->getFileTypeSaveSound();
		ext = string(type == 0 ? ".SND" : ".WAV");
		fileName = mpc::Util::getFileName(nameGui->getName()) + ext;
		if (disk.lock()->checkExists(fileName)) {
			ls.lock()->openScreen("filealreadyexists");
			return;
		}
		disk.lock()->flush();
		disk.lock()->initFiles();
		f = disk.lock()->newFile(fileName);
		if (type == 0) {
			disk.lock()->writeSound(s.lock().get(), f);
		}
		else {
			disk.lock()->writeWav(s.lock().get(), f);
		}
		disk.lock()->flush();
		disk.lock()->initFiles();
		ls.lock()->openScreen("save");
		break;
	}
}
