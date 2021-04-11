#include "LoadScreen.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <disk/SoundLoader.hpp>

#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <file/File.hpp>
#include <file/FileUtil.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::sampler;
using namespace moduru::lang;
using namespace moduru::file;
using namespace std;

LoadScreen::LoadScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "load", layerIndex)
{
}

void LoadScreen::open()
{
	findField("directory").lock()->setLocation(200, 0);
	displayView();

	displayDirectory();
	displayFile();
	displaySize();

	displayFreeSnd();
	findLabel("freeseq").lock()->setText("  2640K");

	auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
	auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
	ls.lock()->setFunctionKeysArrangement(playable ? 1 : 0);
}

void LoadScreen::function(int i)
{
	init();
	
	auto disk = mpc.getDisk().lock();

	switch (i)
	{
	case 1:
		openScreen("save");
		break;
	case 2:
		//openScreen("format");
		break;
	case 3:
		//openScreen("setup");
		break;
	case 4:
	{
		auto controls = mpc.getControls().lock();

		if (controls->isF5Pressed())
		{
			return;
		}

		controls->setF5Pressed(true);

		auto file = getSelectedFile();

		if (!file->isDirectory())
		{
			
			bool started = mpc.getAudioMidiServices().lock()->getSoundPlayer().lock()->start(file->getFile().lock()->getPath());
			
			auto name = file->getFsNode().lock()->getNameWithoutExtension();

			openScreen("popup");
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");

			if (started)
				popupScreen->setText("Playing " + name);
			else
				popupScreen->setText("Can't play " + name);
		}
		break;
	}
	case 5:
	{
		if (!disk || disk->getFileNames().size() == 0)
			return;
		
		auto selectedFile = getSelectedFile();
		auto ext = moduru::file::FileUtil::splitName(selectedFile->getName())[1];
		
		if (StrUtil::eqIgnoreCase(ext, "snd") || StrUtil::eqIgnoreCase(ext, "wav"))
		{
			loadSound();
			return;
		}
		
		if (StrUtil::eqIgnoreCase(ext, "pgm"))
		{
			openScreen("load-a-program");
			return;
		}
		
		if (StrUtil::eqIgnoreCase(ext, "mid"))
		{
			openScreen("load-a-sequence");
			return;
		}
		
		if (StrUtil::eqIgnoreCase(ext, "all"))
		{
			openScreen("mpc2000xl-all-file");
			return;
		}
		
		if (StrUtil::eqIgnoreCase(ext, "aps"))
		{
			openScreen("load-aps-file");
			return;
		}

		if (isSelectedFileDirectory())
		{
			if (disk->moveForward(getSelectedFile()->getName()))
			{
				mpc.getDisk().lock()->initFiles();

				fileLoad = 0;

				displayView();
				displayDirectory();
				displayFile();
				displaySize();

				auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
				auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
				ls.lock()->setFunctionKeysArrangement(playable ? 1 : 0);
			}
		}
		break;
	}
	}
}

void LoadScreen::openWindow()
{
	init();
	auto disk = mpc.getDisk().lock();

	if (!disk)
		return;

	auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
	directoryScreen->previousScreenName = "load";
	directoryScreen->findYOffset0();
	directoryScreen->setYOffset1(fileLoad);
	openScreen("directory");
}

void LoadScreen::turnWheel(int i)
{
	init();

	if (param.compare("view") == 0)
	{
		setView(view + i);
	}
	else if (param.compare("file") == 0)
	{
		setFileLoadWithMaxCheck(fileLoad + i);
	}
	else if (param.compare("directory") == 0)
	{
		auto disk = mpc.getDisk().lock();
		auto currentDir = disk->getDirectoryName();
		auto parents = disk->getParentFiles();
	
		int position = -1;

		for (int j = 0; j < parents.size(); j++)
		{
			if (parents[j]->getName().compare(currentDir) == 0)
			{
				position = j;
				break;
			}
		}

		const int candidate = position + i;

		if (position != -1 && candidate >= 0 && candidate < parents.size())
		{
			if (disk->moveBack())
			{
				disk->initFiles();

				if (disk->moveForward(parents[candidate]->getName()))
				{
					disk->initFiles();
					displayDirectory();
					displayFile();
					displaySize();
				}
				else {
					// From the user's perspective we stay where we are if the above moveForward call fails.
					disk->moveForward(currentDir);
				}
			}
		}
	}

	auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
	auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
	ls.lock()->setFunctionKeysArrangement(playable ? 1 : 0);
}

void LoadScreen::displayView()
{
	findField("view").lock()->setText(views[view]);
}

void LoadScreen::displayDirectory()
{
	findField("directory").lock()->setText(mpc.getDisk().lock()->getDirectoryName());
}

void LoadScreen::displayFreeSnd()
{
	findLabel("freesnd").lock()->setText(" " + StrUtil::padLeft(to_string(mpc.getSampler().lock()->getFreeSampleSpace()), " ", 5) + "K");
}

void LoadScreen::displayFile()
{
	if (mpc.getDisk().lock()->getFileNames().size() == 0)
	{
		findField("file").lock()->setText("");
		return;
	}

	auto selectedFileName = getSelectedFileName();	
	auto selectedFile = getSelectedFile();
	
	if (selectedFileName.length() != 0 && selectedFile && selectedFile->isDirectory())
	{
		findField("file").lock()->setText(u8"\u00C3" + StrUtil::padRight(FileUtil::splitName(selectedFileName)[0], " ", 16));
	}
	else
	{
		auto periodIndex = selectedFileName.find_last_of(".");

		if (periodIndex != string::npos)
		{
			auto extension = selectedFileName.substr(periodIndex, selectedFileName.length());
			auto fileName = StrUtil::padRight(selectedFileName.substr(0, periodIndex), " ", 16);
			selectedFileName = fileName + extension;
		}

		findField("file").lock()->setText(selectedFileName);
	}
}

int LoadScreen::getFileSize()
{
	auto disk = mpc.getDisk().lock();
	
	if (!disk->getFile(fileLoad) || disk->getFile(fileLoad)->isDirectory())
		return 0;
	
	return (int) floor(disk->getFile(fileLoad)->length() / 1024.0);
}

void LoadScreen::displaySize()
{
	if (mpc.getDisk().lock()->getFileNames().size() == 0)
	{
		findLabel("size").lock()->setText("      K");
		return;
	}
	
	findLabel("size").lock()->setText(StrUtil::padLeft(to_string(getFileSize()), " ", 6) + "K");
}

void LoadScreen::setView(int i)
{
	if (i < 0 || i > 8)
		return;
	
	view = i;
	
	mpc.getDisk().lock()->initFiles();
	
	fileLoad = 0;

	displayView();
	displayDirectory();
	displayFile();
	displaySize();
}


shared_ptr<MpcFile> LoadScreen::getSelectedFile()
{
	return mpc.getDisk().lock()->getFile(fileLoad);
}

string LoadScreen::getSelectedFileName()
{
	auto fileNames = mpc.getDisk().lock()->getFileNames();
	
	if (fileNames.size() <= fileLoad)
		return "";

	return fileNames[fileLoad];
}

bool LoadScreen::isSelectedFileDirectory()
{
	return mpc.getDisk().lock()->getFile(fileLoad)->isDirectory();
}

void LoadScreen::setFileLoadWithMaxCheck(int i)
{
	if (i >= mpc.getDisk().lock()->getFileNames().size())
		return;

	setFileLoad(i);
}

void LoadScreen::setFileLoad(int i)
{
	if (i < 0)
		return;

	fileLoad = i;
	displayFile();
	displaySize();
}

void LoadScreen::loadSound()
{
    auto disk = mpc.getDisk().lock();
    disk->setBusy(true);
    SoundLoader soundLoader(mpc, sampler.lock()->getSounds(), false);
    soundLoader.setPreview(true);
    soundLoader.setShowPopup(true);

    SoundLoaderResult result;
    
    try
    {
        soundLoader.loadSound(getSelectedFile(), result);
    }
    catch (const exception& exception)
    {
        sampler.lock()->deleteSound(sampler.lock()->getSoundCount() - 1);
        
        MLOG("A problem occurred when trying to load " + getSelectedFileName() + ": " + string(exception.what()));
        MLOG(result.errorMessage);
        disk->setBusy(false);
        openScreen("load");
    }

    auto popupScreen = mpc.screens->get<PopupScreen>("popup");

    if (!result.success)
    {
        sampler.lock()->deleteSound(sampler.lock()->getSoundCount() - 1);
        disk->setBusy(false);
        openScreen("popup");
        popupScreen->setText(result.errorMessage);
        popupScreen->returnToScreenAfterMilliSeconds("load", 500);
        return;
    }
    
    if (result.existingIndex != -1)
    {
        sampler.lock()->deleteSound(sampler.lock()->getSoundCount() - 1);
    }
    else
    {
        mpc.getLayeredScreen().lock()->openScreen("popup");
        auto name = FileUtil::splitName(getSelectedFileName())[0];
        auto ext = FileUtil::splitName(getSelectedFileName())[1];
        popupScreen->setText("LOADING " + StrUtil::padRight(name, " ", 16) + "." + ext);
     
        int sleepTime = soundLoader.getSize() / 400;
        if (sleepTime < 300) sleepTime = 300;
        
        popupScreen->returnToScreenAfterMilliSeconds("load-a-sound", sleepTime);
    }

    // PopupScreen should have a std::function delayedAction; that does the below
    disk->setBusy(false);
}
