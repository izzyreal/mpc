#include "LoadScreen.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>

#include <disk/SoundLoader.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/Volume.hpp>

#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceScreen.hpp>
#include <lcdgui/screens/window/VmpcConvertAndLoadWavScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <nvram/VolumesPersistence.hpp>

#include <file/FileUtil.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::sampler;
using namespace moduru::lang;
using namespace moduru::file;

LoadScreen::LoadScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "load", layerIndex)
{
}

void LoadScreen::open()
{
    if (ls->getPreviousScreenName() != "popup")
    {
        device = mpc.getDiskController()->getActiveDiskIndex();
    }
    
	findField("directory")->setLocation(200, 0);
	displayView();

	displayDirectory();
	displayFile();
	displaySize();
    displayDevice();
    displayDeviceType();

	displayFreeSnd();
	findLabel("freeseq")->setText("  2640K");

	auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
	auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
    
    init();
    
    if (param == "device")
        ls->setFunctionKeysArrangement(device == mpc.getDiskController()->activeDiskIndex ? 0 : 2);
    else
        ls->setFunctionKeysArrangement(playable ? 1 : 0);
}

void LoadScreen::function(int i)
{
	init();
	
	auto disk = mpc.getDisk();

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
        if (param == "device")
        {
            if (mpc.getDiskController()->activeDiskIndex == device)
                return;

            auto& candidateVolume = mpc.getDisks()[device]->getVolume();
            
            if (candidateVolume.mode == DISABLED)
            {
                auto popupScreen = mpc.screens->get<PopupScreen>("popup");
                popupScreen->setText("Device is disabled in DISKS");
                popupScreen->returnToScreenAfterMilliSeconds("load", 2000);
                openScreen("popup");
                return;
            }
            
			auto oldIndex = mpc.getDiskController()->activeDiskIndex;

            mpc.getDiskController()->activeDiskIndex = device;
            auto newDisk = mpc.getDisk();

            fileLoad = 0;
            
			if (newDisk->getVolume().type== USB_VOLUME) {
                
                newDisk->initRoot();
                
                if (!newDisk->getVolume().volumeStream.is_open()) {
					mpc.getDiskController()->activeDiskIndex = oldIndex;
					auto popupScreen = mpc.screens->get<PopupScreen>("popup");
					popupScreen->setText("Error! Device seems in use");
					popupScreen->returnToScreenAfterMilliSeconds("load", 2000);
					openScreen("popup");
					return;
				}
            }
			
            ls->setFunctionKeysArrangement(0);
            
			newDisk->initFiles();
            
            displayFile();
            displaySize();
            displayDirectory();
            displayDevice();
            displayDeviceType();
            
            mpc::nvram::VolumesPersistence::save(mpc);
            
            return;
        }
        
		auto controls = mpc.getControls();

		if (controls->isF5Pressed())
		{
			return;
		}

		controls->setF5Pressed(true);

		auto file = getSelectedFile();
        
		if (!file->isDirectory())
		{
            
            auto ext = FileUtil::splitName(file->getName())[1];
            
            bool isWav = StrUtil::eqIgnoreCase(ext, "wav");
            bool isSnd = StrUtil::eqIgnoreCase(ext, "snd");

            if (!isWav && !isSnd) return;
                
			bool started = mpc.getAudioMidiServices()->getSoundPlayer()->start(
                    file->getInputStream(),
                    isSnd ? audiomidi::SoundPlayerFileFormat::SND : audiomidi::SoundPlayerFileFormat::WAV);
            
			auto name = file->getNameWithoutExtension();

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
            bool shouldBeConverted = false;
			loadSound(shouldBeConverted);
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
				mpc.getDisk()->initFiles();

				fileLoad = 0;

				displayView();
				displayDirectory();
				displayFile();
				displaySize();

				auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
				auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
				ls->setFunctionKeysArrangement(playable ? 1 : 0);
			}
		}
		break;
	}
	}
}

void LoadScreen::openWindow()
{
	init();
	auto disk = mpc.getDisk();

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

	if (param == "view")
	{
		setView(view + i);
	}
	else if (param == "file")
	{
		setFileLoadWithMaxCheck(fileLoad + i);
	}
	else if (param == "directory")
	{
		auto disk = mpc.getDisk();
		auto currentDir = disk->getDirectoryName();
		auto parents = disk->getParentFileNames();
	
		int position = -1;

		for (int j = 0; j < parents.size(); j++)
		{
			if (parents[j] == currentDir)
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

				if (disk->moveForward(parents[candidate]))
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
    else if (param == "device")
    {
        if (device + i < 0 || device + i >= mpc.getDisks().size())
            return;
        
        device += i;
        displayDevice();
        displayDeviceType();
        ls->setFunctionKeysArrangement(mpc.getDiskController()->activeDiskIndex == device ? 0 : 2);
        return;
    }

	auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
	auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
	ls->setFunctionKeysArrangement(playable ? 1 : 0);
}

void LoadScreen::displayView()
{
	findField("view")->setText(views[view]);
}

void LoadScreen::displayDirectory()
{
	findField("directory")->setText(mpc.getDisk()->getDirectoryName());
}

void LoadScreen::displayFreeSnd()
{
	findLabel("freesnd")->setText(" " + StrUtil::padLeft(std::to_string(sampler->getFreeSampleSpace()), " ", 5) + "K");
}

void LoadScreen::displayFile()
{
	if (mpc.getDisk()->getFileNames().size() == 0)
	{
		findField("file")->setText("");
		return;
	}

	auto selectedFileName = getSelectedFileName();	
	auto selectedFile = getSelectedFile();
	
	if (selectedFileName.length() != 0 && selectedFile && selectedFile->isDirectory())
	{
		findField("file")->setText(u8"\u00C3" + StrUtil::padRight(FileUtil::splitName(selectedFileName)[0], " ", 16));
	}
	else
	{
		auto periodIndex = selectedFileName.find_last_of(".");

		if (periodIndex != std::string::npos)
		{
			auto extension = selectedFileName.substr(periodIndex, selectedFileName.length());
			auto fileName = StrUtil::padRight(selectedFileName.substr(0, periodIndex), " ", 16);
			selectedFileName = fileName + extension;
		}

		findField("file")->setText(selectedFileName);
	}
}

int LoadScreen::getFileSize()
{
	auto disk = mpc.getDisk();
	
	if (!disk->getFile(fileLoad) || disk->getFile(fileLoad)->isDirectory())
		return 0;
	
	return (int) floor(disk->getFile(fileLoad)->length() / 1024.0);
}

void LoadScreen::displaySize()
{
	if (mpc.getDisk()->getFileNames().size() == 0)
	{
		findLabel("size")->setText("      K");
		return;
	}
	
	findLabel("size")->setText(StrUtil::padLeft(std::to_string(getFileSize()), " ", 6) + "K");
}

void LoadScreen::setView(int i)
{
	if (i < 0 || i > 8)
		return;
	
	view = i;
	
	mpc.getDisk()->initFiles();
	
	fileLoad = 0;

	displayView();
	displayDirectory();
	displayFile();
	displaySize();
}


std::shared_ptr<MpcFile> LoadScreen::getSelectedFile()
{
	return mpc.getDisk()->getFile(fileLoad);
}

std::string LoadScreen::getSelectedFileName()
{
	auto fileNames = mpc.getDisk()->getFileNames();
	
	if (fileNames.size() <= fileLoad)
		return "";

	return fileNames[fileLoad];
}

bool LoadScreen::isSelectedFileDirectory()
{
	return mpc.getDisk()->getFile(fileLoad)->isDirectory();
}

void LoadScreen::setFileLoadWithMaxCheck(int i)
{
	if (i >= mpc.getDisk()->getFileNames().size())
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

void LoadScreen::loadSound(bool shouldBeConverted)
{
    SoundLoader soundLoader(mpc, sampler->getSounds(), false);
    soundLoader.setPreview(true);

    SoundLoaderResult result;

    auto sound = sampler->addSound();

    soundLoader.loadSound(getSelectedFile(), result, sound, shouldBeConverted);

    auto popupScreen = mpc.screens->get<PopupScreen>("popup");

    if (result.success)
    {
        ls->openScreen("popup");
        auto name = FileUtil::splitName(getSelectedFileName())[0];
        auto ext = FileUtil::splitName(getSelectedFileName())[1];
        popupScreen->setText("LOADING " + StrUtil::padRight(name, " ", 16) + "." + ext);
        popupScreen->returnToScreenAfterMilliSeconds("load-a-sound", 300);
        return;
    }

    sampler->deleteSound(sound);

    if (result.canBeConverted) {
        auto loadRoutine = [&]() {
            const bool shouldBeConverted2 = true;
            loadSound(shouldBeConverted2);
        };

        auto convertAndLoadWavScreen = mpc.screens->get<VmpcConvertAndLoadWavScreen>("vmpc-convert-and-load-wav");
        convertAndLoadWavScreen->setLoadRoutine(loadRoutine);
        openScreen("vmpc-convert-and-load-wav");
    } else {
        openScreen("popup");
        popupScreen->setText(result.errorMessage);
        popupScreen->returnToScreenAfterMilliSeconds("load", 700);
    }
}

void LoadScreen::displayDevice()
{
    auto dev = findChild<Field>("device");
    dev->setText(mpc.getDisks()[device]->getVolume().label);
}

void LoadScreen::displayDeviceType()
{
    auto type = findChild<Label>("device-type");
    type->setText(mpc.getDisks()[device]->getVolume().typeShortName());
}

void LoadScreen::up()
{
	init();
    
	if (param == "device")
	{
		device = mpc.getDiskController()->activeDiskIndex;
		displayDevice();
		auto splitFileName = StrUtil::split(getSelectedFileName(), '.');
		auto playable = splitFileName.size() > 1 && (StrUtil::eqIgnoreCase(splitFileName[1], "snd") || StrUtil::eqIgnoreCase(splitFileName[1], "wav"));
		ls->setFunctionKeysArrangement(playable ? 1 : 0);
	}

	ScreenComponent::up();
}
