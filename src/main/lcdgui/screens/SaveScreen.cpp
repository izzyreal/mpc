#include "SaveScreen.hpp"

#include <cstdint>

#include <sequencer/Track.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/window/SaveAllFileScreen.hpp>
#include <lcdgui/screens/window/SaveApsFileScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/Volume.hpp>
#include <nvram/VolumesPersistence.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

SaveScreen::SaveScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "save", layerIndex)
{
}

void SaveScreen::open()
{
    mpc.getDisk()->initFiles();

    for (int i = 0; i < 24; i++)
    {
        if (sampler->getProgram(i))
        {
            programIndex = i;
            break;
        }
    }

    findField("directory")->setLocation(200, 0);

    displaySize();
    displayType();
    displayFile();
    displayFree();
    displayDirectory();
    displayDevice();
    displayDeviceType();

    init();

    if (param == "device")
    {
        findChild<FunctionKeys>("function-keys")->setActiveArrangement(device == mpc.getDiskController()->getActiveDiskIndex() ? 0 : 1);
    }
    else
    {
        findChild<FunctionKeys>("function-keys")->setActiveArrangement(0);
    }
}

void SaveScreen::openWindow()
{
    init();

    if (param == "directory")
    {
        mpc.getLayeredScreen()->openScreen<DirectoryScreen>();
    }
}

void SaveScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 0:
        mpc.getLayeredScreen()->openScreen<LoadScreen>();
            break;
        case 2:
            //openScreen<FormatScreen>();
            break;
        case 3:
            //openScreen<SetupScreen>();
            break;
        case 4:
            if (param == "device")
            {
                if (mpc.getDiskController()->getActiveDiskIndex() == device)
                    return;

                auto& candidateVolume = mpc.getDisks()[device]->getVolume();

                if (candidateVolume.mode == mpc::disk::MountMode::DISABLED)
                {
                    ls->showPopupForMs("Device is disabled in DISKS", 2000);
                    return;
                }

                auto oldIndex = mpc.getDiskController()->getActiveDiskIndex();

                mpc.getDiskController()->setActiveDiskIndex(device);
                auto newDisk = mpc.getDisk();

                if (newDisk->getVolume().type== mpc::disk::VolumeType::USB_VOLUME) {

                    newDisk->initRoot();

                    if (!newDisk->getVolume().volumeStream.is_open())
                    {
                        mpc.getDiskController()->setActiveDiskIndex(oldIndex);
                        ls->showPopupForMs("Error! Device seems in use", 2000);
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
        case 5:
        {
            switch (type)
            {
                case 0:
                {
        mpc.getLayeredScreen()->openScreen<SaveAllFileScreen>();
                    break;
                }
                case 1:
                    if (!sequencer.lock()->getActiveSequence()->isUsed())
                    {
                        return;
                    }
                    
        mpc.getLayeredScreen()->openScreen<SaveASequenceScreen>();
                    break;
                case 2:
        mpc.getLayeredScreen()->openScreen<SaveApsFileScreen>();
                    break;
                case 3:
        mpc.getLayeredScreen()->openScreen<SaveAProgramScreen>();
                    break;
                case 4:
                    if (sampler->getSoundCount() == 0)
                    {
                        break;
                    }

        mpc.getLayeredScreen()->openScreen<SaveASoundScreen>();
                    break;
            }
        }
    }
}

void SaveScreen::turnWheel(int i)
{
    init();
    
    if (param == "type")
    {
        setType(type + i);
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
    else if (param == "file")
    {
        switch (type)
        {
            case 1:
                sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
                break;
            case 3:
            {
                unsigned char counter = 0;

                for (int idx = programIndex;
                     (i < 0) ? idx >= 0 : idx < 24;
                     (i < 0) ? idx-- : idx++)
                {
                    if (sampler->getProgram(idx))
                    {
                        programIndex = idx;
                    }
                    else
                    {
                        continue;
                    }

                    if (++counter == abs(i) + 1)
                    {
                        break;
                    }
                }
                break;
            }
            case 4:
                sampler->setSoundIndex(sampler->getSoundIndex() + i);
                break;
        }
        displayFile();
        displaySize();
    }
    else if (param == "device")
    {
        if (device + i < 0 || device + i >= mpc.getDisks().size())
            return;

        device += i;
        displayDevice();
        displayDeviceType();
        ls->setFunctionKeysArrangement(mpc.getDiskController()->getActiveDiskIndex() == device ? 0 : 1);
        return;
    }
}

void SaveScreen::setType(int i)
{
    if (i < 0 || i > 4)
        return;
    
    type = i;

    if (i == 3)
    {
        for (int j = 0; j < 24; j++)
        {
            if (sampler->getProgram(j))
            {
                programIndex = j;
                break;
            }
        }
    }

    displayType();
    displayFile();
    displaySize();
}

void SaveScreen::displayType()
{
    findField("type")->setText(types[type]);
}

void SaveScreen::displayFile()
{
    std::string fileName;
    
    switch (type)
    {
        case 0:
        {
            const auto saveAllFileScreen = mpc.screens->get<SaveAllFileScreen>();
            fileName = saveAllFileScreen->fileName;
            break;
        }
        case 1:
        {
            auto num = StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2);
            name = sequencer.lock()->getActiveSequence()->getName();
            fileName = num + "-" + name;
            break;
        }
        case 2:
        {
            const auto saveApsFileScreen = mpc.screens->get<SaveApsFileScreen>();
            fileName = saveApsFileScreen->fileName;
            break;
        }
        case 3:
            fileName = sampler->getProgram(programIndex)->getName();
            break;
        case 4:
            fileName = std::string(sampler->getSoundCount() == 0 ? " (No sound)" : sampler->getSound()->getName());
            break;
        case 5:
            fileName = "MPC2KXL         .BIN";
            break;
    }
    
    findField("file")->setText(fileName);
}

void SaveScreen::displaySize()
{
    auto seq = sequencer.lock()->getActiveSequence();
    auto size = 0;
    
    switch (type)
    {
        case 0:
            size = sequencer.lock()->getUsedSequenceCount() * 25;
            break;
        case 1:
            size = seq->isUsed() ? 10 + static_cast<int>(seq->getEventCount() * 0.001) : -1;
            break;
        case 2:
            size = sampler->getProgramCount() * 4;
            break;
        case 3:
            size = 4;
            break;
        case 4:
            size = sampler->getSoundCount() == 0 ? -1 : (sampler->getSound()->getSampleData()->size() * 2 * 0.001);
            break;
        case 5:
            size = 512;
            break;
    }
    
    findLabel("size")->setText(StrUtil::padLeft(std::to_string(size == -1 ? 0 : size), " ", 6) + "K");
}

void SaveScreen::displayFree()
{
    std::uintmax_t availableSpaceInBytes = 0;

    try {
        availableSpaceInBytes = fs::space(mpc.paths->storesPath()).available;
    } catch (fs::filesystem_error&) {
        MLOG("An exception occurred when SaveScreen::displayFree was trying to query available space!");
    }

    const auto text = byte_count_to_short_string(availableSpaceInBytes);
    findLabel("free")->setText(text);
}

void SaveScreen::displayDirectory()
{
    findField("directory")->setText(mpc.getDisk()->getDirectoryName());
}

void SaveScreen::displayDevice()
{
    auto dev = findChild<Field>("device");
    dev->setText(mpc.getDisks()[device]->getVolume().label);
}

void SaveScreen::displayDeviceType()
{
    auto deviceTypeLabel = findChild<Label>("device-type");
    deviceTypeLabel->setText(mpc.getDisks()[device]->getVolume().typeShortName());
}

void SaveScreen::up()
{
    init();

    if (param == "device")
    {
        device = mpc.getDiskController()->getActiveDiskIndex();
        displayDevice();
        ls->setFunctionKeysArrangement(0);
    }

    ScreenComponent::up();
}

unsigned char SaveScreen::getProgramIndex()
{
    return programIndex;
}
