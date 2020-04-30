#include <controls/disk/dialog/FileAlreadyExistsControls.hpp>

#include <Util.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <file/all/AllParser.hpp>
#include <file/aps/ApsSaver.hpp>
#include <ui/NameGui.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::disk::dialog;
using namespace std;

FileAlreadyExistsControls::FileAlreadyExistsControls() 
	: AbstractDiskControls()
{
}

void FileAlreadyExistsControls::function(int i)
{
	super::function(i);
	auto lDisk = disk.lock();
	switch (i) {
	case 2:
		if (ls.lock()->getPreviousScreenName().compare("saveaprogram") == 0) {
			auto pfileName = mpc::Util::getFileName(nameGui->getName()) + ".PGM";
			auto success = lDisk->getFile(pfileName)->del();
			if(success) {
				lDisk->flush();
				lDisk->initFiles();
				lDisk->writeProgram(program.lock().get(), pfileName);
			}
			ls.lock()->openScreen("save");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveasequence") == 0) {
			auto sfileName = mpc::Util::getFileName(nameGui->getName()) + ".MID";
			auto success = lDisk->getFile(sfileName)->del();
			if(success) {
				lDisk->flush();
				lDisk->initFiles();
				lDisk->writeSequence(sequencer.lock()->getActiveSequence().lock().get(), sfileName);
				ls.lock()->openScreen("save");
			}
			ls.lock()->openScreen("save");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveapsfile") == 0) {
			auto apsName = mpc::Util::getFileName(nameGui->getName()) + ".APS";
			auto success = lDisk->getFile(apsName)->del();
			if(success) {
				lDisk->flush();
				lDisk->initFiles();
				mpc::file::aps::ApsSaver apsSaver(apsName);
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveallfile") == 0) {
			auto allName = mpc::Util::getFileName(nameGui->getName()) + ".ALL";
			lDisk->initFiles();
			auto success = lDisk->getFile(allName)->del();
			if(success) {
				lDisk->flush();
				lDisk->initFiles();
				auto allParser = mpc::file::all::AllParser(mpc::Util::getFileName(nameGui->getName()));
				auto f = lDisk->newFile(allName);
                auto bytes = allParser.getBytes();
				f->setFileData(&bytes);
				lDisk->flush();
				lDisk->initFiles();
				ls.lock()->openScreen("save");
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveasound") == 0) {
			auto s = sampler.lock()->getSound(soundGui->getSoundIndex());
			auto type = diskGui->getFileTypeSaveSound();
			auto ext = string(type == 0 ? ".SND" : ".WAV");
			auto fileName = mpc::Util::getFileName(nameGui->getName()) + ext;
			lDisk->getFile(fileName)->del();
			lDisk->flush();
			lDisk->initFiles();
			auto f = lDisk->newFile(fileName);
			if(type == 0) {
				disk.lock()->writeSound(dynamic_pointer_cast<mpc::sampler::Sound>(s.lock()).get(), f);
			} else {
				disk.lock()->writeWav(dynamic_pointer_cast<mpc::sampler::Sound>(s.lock()).get(), f);
			}
			lDisk->flush();
			lDisk->initFiles();
			ls.lock()->openScreen("save");
		}
		break;
	case 4:
		if (ls.lock()->getPreviousScreenName().compare("saveaprogram") == 0) {
			nameGui->setParameterName("savingpgm");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveasequence") == 0) {
			nameGui->setParameterName("savingmid");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveapsfile") == 0) {
			nameGui->setParameterName("savingaps");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveallfile") == 0) {
			nameGui->setParameterName("saveallfile");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("saveasound") == 0) {
			nameGui->setParameterName("saveasound");
			ls.lock()->openScreen("name");
		}
		break;
	}
}
