#include "FileAlreadyExistsScreen.hpp"

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <file/all/AllParser.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/SaveASoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;
using namespace std;

FileAlreadyExistsScreen::FileAlreadyExistsScreen(const int layerIndex) 
	: ScreenComponent("file-already-exists", layerIndex)
{
}

void FileAlreadyExistsScreen::function(int i)
{
	BaseControls::function(i);
		
	switch (i)
	{
	case 2:
	{
		auto disk = mpc.getDisk().lock();

		if (ls.lock()->getPreviousScreenName().compare("saveaprogram") == 0)
		{
			auto pfileName = mpc::Util::getFileName(nameGui->getName()) + ".PGM";
			auto success = disk->getFile(pfileName)->del();

			if (success)
			{
				disk->flush();
				disk->initFiles();
				disk->writeProgram(program.lock().get(), pfileName);
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sequence") == 0)
		{
			auto sfileName = mpc::Util::getFileName(nameGui->getName()) + ".MID";
			auto success = disk->getFile(sfileName)->del();

			if (success)
			{
				disk->flush();
				disk->initFiles();
				disk->writeSequence(sequencer.lock()->getActiveSequence().lock().get(), sfileName);
				ls.lock()->openScreen("save");
			}
			ls.lock()->openScreen("save");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-aps-file") == 0)
		{
			auto apsName = mpc::Util::getFileName(nameGui->getName()) + ".APS";
			auto success = disk->getFile(apsName)->del();
			
			if (success)
			{
				disk->flush();
				disk->initFiles();
				apsSaver = make_unique<mpc::disk::ApsSaver>(apsName);
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-all-file") == 0)
		{
			auto allName = mpc::Util::getFileName(nameGui->getName()) + ".ALL";
			disk->initFiles();
			auto success = disk->getFile(allName)->del();
			
			if (success)
			{
				disk->flush();
				disk->initFiles();
				auto allParser = mpc::file::all::AllParser(mpc::Util::getFileName(nameGui->getName()));
				auto f = disk->newFile(allName);
				auto bytes = allParser.getBytes();
				f->setFileData(&bytes);
				disk->flush();
				disk->initFiles();
				ls.lock()->openScreen("save");
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sound") == 0)
		{
			auto s = sampler.lock()->getSound().lock();

			auto saveASoundScreen = dynamic_pointer_cast<SaveASoundScreen>(Screens::getScreenComponent("save-a-sound"));

			auto type = saveASoundScreen->fileType;

			auto ext = string(type == 0 ? ".SND" : ".WAV");
			auto fileName = mpc::Util::getFileName(nameGui->getName()) + ext;

			disk->getFile(fileName)->del();
			disk->flush();
			disk->initFiles();

			auto f = disk->newFile(fileName);

			if (type == 0)
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
		}
		break;
	}
	case 4:
		if (ls.lock()->getPreviousScreenName().compare("saveaprogram") == 0)
		{
			nameGui->setParameterName("savingpgm");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sequence") == 0)
		{
			nameGui->setParameterName("savingmid");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-aps-file") == 0)
		{
			nameGui->setParameterName("savingaps");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-all-file") == 0)
		{
			nameGui->setParameterName("save-all-file");
			ls.lock()->openScreen("name");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sound") == 0)
		{
			nameGui->setParameterName("save-a-sound");
			ls.lock()->openScreen("name");
		}
		break;
	}
}
