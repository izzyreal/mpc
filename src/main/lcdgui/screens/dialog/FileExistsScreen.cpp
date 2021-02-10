#include "FileExistsScreen.hpp"

#include <Util.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <file/all/AllParser.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/SaveASoundScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;
using namespace std;

FileExistsScreen::FileExistsScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "file-exists", layerIndex)
{
}

void FileExistsScreen::function(int i)
{
	ScreenComponent::function(i);
		
	switch (i)
	{
	case 2:
	{
		auto disk = mpc.getDisk().lock();
		auto nameScreen = mpc.screens->get<NameScreen>("name");

		if (ls.lock()->getPreviousScreenName().compare("save-a-program") == 0)
		{
			auto pfileName = mpc::Util::getFileName(nameScreen->getName()) + ".PGM";
			auto success = disk->getFile(pfileName)->del();

			if (success)
			{
				disk->flush();
				disk->initFiles();
				disk->writeProgram(program, pfileName);
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sequence") == 0)
		{
			auto sfileName = mpc::Util::getFileName(nameScreen->getName()) + ".MID";
			auto success = disk->getFile(sfileName)->del();

			if (success)
			{
				disk->flush();
				disk->initFiles();
				disk->writeSequence(sequencer.lock()->getActiveSequence(), sfileName);
				openScreen("save");
			}
			openScreen("save");
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-aps-file") == 0)
		{
			auto apsName = mpc::Util::getFileName(nameScreen->getName()) + ".APS";
			auto success = disk->getFile(apsName)->del();
			
			if (success)
			{
				disk->flush();
				disk->initFiles();
				apsSaver = make_unique<mpc::disk::ApsSaver>(mpc, apsName);
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-all-file") == 0)
		{
			auto allName = mpc::Util::getFileName(nameScreen->getName()) + ".ALL";
			disk->initFiles();
			auto success = disk->getFile(allName)->del();
			
			if (success)
			{
				disk->flush();
				disk->initFiles();
				auto allParser = mpc::file::all::AllParser(mpc, mpc::Util::getFileName(nameScreen->getName()));
				auto f = disk->newFile(allName);
				auto bytes = allParser.getBytes();
				f->setFileData(&bytes);
				disk->flush();
				disk->initFiles();
				openScreen("save");
			}
		}
		else if (ls.lock()->getPreviousScreenName().compare("save-a-sound") == 0)
		{
			auto s = sampler.lock()->getSound().lock();

			auto saveASoundScreen = mpc.screens->get<SaveASoundScreen>("save-a-sound");

			auto type = saveASoundScreen->fileType;

			auto ext = string(type == 0 ? ".SND" : ".WAV");
			auto fileName = mpc::Util::getFileName(nameScreen->getName()) + ext;

			disk->getFile(fileName)->del();
			disk->flush();
			disk->initFiles();

			auto f = disk->newFile(fileName);

			if (type == 0)
				disk->writeSound(s, f);
			else
				disk->writeWav(s, f);

			disk->flush();
			disk->initFiles();
			openScreen("save");
		}
		break;
	}
	case 4:
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");

		vector<string> screens{ "save-a-program", "save-a-sequence", "save-aps-file", "save-all-file", "save-a-sound" };

		auto previousScreen = ls.lock()->getPreviousScreenName();

		if (find(begin(screens), end(screens), nameScreen->parameterName) != end(screens))
			openScreen("name");

		if (find(begin(screens), end(screens), previousScreen) != end(screens))
		{
			nameScreen->parameterName = previousScreen;
			openScreen("name");
		}

		break;
	}
	}
}
