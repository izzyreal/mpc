#include "LoadScreen.hpp"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include "engine/audio/server/NonRealTimeAudioServer.hpp"

#include <disk/SoundLoader.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/MpcFile.hpp>

#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceScreen.hpp>
#include <lcdgui/screens/window/VmpcConvertAndLoadWavScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <nvram/VolumesPersistence.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::sampler;

LoadScreen::LoadScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "load", layerIndex)
{
}

void LoadScreen::open()
{
    mpc.getDisk()->initFiles();

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

	auto ext = fs::path(getSelectedFileName()).extension().string();
	auto playable = StrUtil::eqIgnoreCase(ext, ".snd") || StrUtil::eqIgnoreCase(ext, ".wav");
    
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
            
            auto ext = fs::path(file->getName()).extension().string();
            
            bool isWav = StrUtil::eqIgnoreCase(ext, ".wav");
            bool isSnd = StrUtil::eqIgnoreCase(ext, ".snd");

            if (!isWav && !isSnd) return;

            const auto audioServerSampleRate = mpc.getAudioMidiServices()->getAudioServer()->getSampleRate();

			bool started = mpc.getAudioMidiServices()->getSoundPlayer()->start(
                    file->getInputStream(),
                    isSnd ? audiomidi::SoundPlayerFileFormat::SND : audiomidi::SoundPlayerFileFormat::WAV,
                    audioServerSampleRate);
            
			auto name = file->getNameWithoutExtension();

			openScreen("popup");
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");

			if (started)
            {
                popupScreen->setText("Playing " + name);
            }
			else
            {
                popupScreen->setText("Can't play " + name);
            }
		}
		break;
	}
	case 5:
	{
		if (!disk || disk->getFileNames().empty())
        {
            return;
        }
		
		auto selectedFile = getSelectedFile();
		auto ext = fs::path(selectedFile->getName()).extension().string();

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

                auto ext = fs::path(getSelectedFileName()).extension().string();
                auto playable = StrUtil::eqIgnoreCase(ext, ".snd") || StrUtil::eqIgnoreCase(ext, ".wav");
                ls->setFunctionKeysArrangement(playable ? 1 : 0);
            }
        }
		else if (StrUtil::eqIgnoreCase(ext, ".snd") || StrUtil::eqIgnoreCase(ext, ".wav"))
		{
            const bool shouldBeConverted = false;
			loadSound(shouldBeConverted);
		}
		else if (StrUtil::eqIgnoreCase(ext, ".pgm"))
		{
			openScreen("load-a-program");
		}
		else if (StrUtil::eqIgnoreCase(ext, ".mid"))
		{
			openScreen("load-a-sequence");
		}
		else if (StrUtil::eqIgnoreCase(ext, ".all"))
		{
			openScreen("mpc2000xl-all-file");
		}
		else if (StrUtil::eqIgnoreCase(ext, ".aps"))
		{
			openScreen("load-aps-file");
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

	auto newSelectedFileExtension = fs::path(getSelectedFileName()).extension().string();
	auto playable = StrUtil::eqIgnoreCase(newSelectedFileExtension, ".snd") || StrUtil::eqIgnoreCase(newSelectedFileExtension, ".wav");
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
	if (mpc.getDisk()->getFileNames().empty())
	{
		findField("file")->setText("");
		return;
	}

	auto selectedFileName = getSelectedFileName();	
	auto selectedFile = getSelectedFile();
	
	if (selectedFileName.length() != 0 && selectedFile && selectedFile->isDirectory())
	{
		findField("file")->setText(u8"\u00C3" + StrUtil::padRight(fs::path(selectedFileName).stem().string(), " ", 16));
	}
	else
	{
		auto periodIndex = selectedFileName.find_last_of('.');

		if (periodIndex != std::string::npos)
		{
			auto extension = selectedFileName.substr(periodIndex, selectedFileName.length());
			auto fileName = StrUtil::padRight(selectedFileName.substr(0, periodIndex), " ", 16);
			selectedFileName = fileName + extension;
		}

		findField("file")->setText(selectedFileName);
	}
}

unsigned long LoadScreen::getFileSizeKb()
{
	auto file = getSelectedFile();
	
	if (!file || file->isDirectory())
    {
        return 0;
    }

	return static_cast<unsigned long>(ceil(file->length() / 1024.0));
}

void LoadScreen::displaySize()
{
	if (mpc.getDisk()->getFileNames().empty())
	{
		findLabel("size")->setText("      K");
		return;
	}

    auto candidate = getFileSizeKb();

    std::vector<std::string> units{"K", "M", "G", "T", "?"};

    unsigned char counter = 0;

    std::string sizeText;

    while ((sizeText = std::to_string(candidate)).length() > 6 && counter <= 4)
    {
        candidate /= 1024;
        counter++;
    }

    if (sizeText.length() > 6)
    {
        sizeText = sizeText.substr(0, 6);
    }

	findLabel("size")->setText(StrUtil::padLeft(sizeText, " ", 6) + units[counter]);
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
	
	if (fileLoad >= fileNames.size())
    {
        return "";
    }

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
    SoundLoader soundLoader(mpc, false);
    soundLoader.setPreview(true);

    SoundLoaderResult result;

    auto sound = sampler->addSound("load");

    if (sound == nullptr)
    {
        return;
    }

    soundLoader.loadSound(getSelectedFile(), result, sound, shouldBeConverted);

    auto popupScreen = mpc.screens->get<PopupScreen>("popup");

    if (result.success)
    {
        ls->openScreen("popup");
        auto path = fs::path(getSelectedFileName());
        auto name = path.stem().string();
        auto ext = path.extension().string();
        popupScreen->setText("LOADING " + StrUtil::padRight(name, " ", 16) + ext);
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
		auto ext = fs::path(getSelectedFileName()).extension().string();
		auto playable = StrUtil::eqIgnoreCase(ext, ".snd") || StrUtil::eqIgnoreCase(ext, ".wav");
		ls->setFunctionKeysArrangement(playable ? 1 : 0);
	}

	ScreenComponent::up();
}
