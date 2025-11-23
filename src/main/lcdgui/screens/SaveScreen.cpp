#include "SaveScreen.hpp"

#include "Mpc.hpp"
#include "Logger.hpp"
#include "StrUtil.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"
#include "lcdgui/screens/window/SaveApsFileScreen.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/Volume.hpp"
#include "lcdgui/FunctionKeys.hpp"
#include "lcdgui/Label.hpp"
#include "nvram/VolumesPersistence.hpp"

#include <cstdint>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

SaveScreen::SaveScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save", layerIndex)
{
}

void SaveScreen::open()
{
    mpc.getDisk()->initFiles();

    if (ls.lock()->isPreviousScreenNot({ScreenId::PopupScreen}))
    {
        device = mpc.getDiskController()->getActiveDiskIndex();
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; i++)
    {
        if (sampler.lock()->getProgram(i)->isUsed())
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

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "device")
    {
        findChild<FunctionKeys>("function-keys")
            ->setActiveArrangement(
                device == mpc.getDiskController()->getActiveDiskIndex() ? 0
                                                                        : 1);
    }
    else
    {
        findChild<FunctionKeys>("function-keys")->setActiveArrangement(0);
    }
}

void SaveScreen::openWindow()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "directory")
    {
        openScreenById(ScreenId::DirectoryScreen);
    }
}

void SaveScreen::function(const int i)
{
    switch (i)
    {
        case 0:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 2:
            // openScreen<FormatScreen>();
            break;
        case 3:
            // openScreen<SetupScreen>();
            break;
        case 4:
        {
            const auto focusedFieldName = getFocusedFieldNameOrThrow();

            if (focusedFieldName == "device")
            {
                if (mpc.getDiskController()->getActiveDiskIndex() == device)
                {
                    return;
                }

                const auto &candidateVolume =
                    mpc.getDisks()[device]->getVolume();

                if (candidateVolume.mode == disk::MountMode::DISABLED)
                {
                    ls.lock()->showPopupForMs("Device is disabled in DISKS",
                                              1000);
                    return;
                }

                const auto oldIndex =
                    mpc.getDiskController()->getActiveDiskIndex();

                mpc.getDiskController()->setActiveDiskIndex(device);
                const auto newDisk = mpc.getDisk();

                if (newDisk->getVolume().type == disk::VolumeType::USB_VOLUME)
                {

                    newDisk->initRoot();

                    if (!newDisk->getVolume().volumeStream.is_open())
                    {
                        mpc.getDiskController()->setActiveDiskIndex(oldIndex);
                        ls.lock()->showPopupForMs("Error! Device seems in use",
                                                  2000);
                        return;
                    }
                }

                ls.lock()->setFunctionKeysArrangement(0);

                newDisk->initFiles();

                displayFile();
                displaySize();
                displayDirectory();
                displayDevice();
                displayDeviceType();

                nvram::VolumesPersistence::save(mpc);

                return;
            }
        }
        case 5:
        {
            switch (type)
            {
                case 0:
                {
                    openScreenById(ScreenId::SaveAllFileScreen);
                    break;
                }
                case 1:
                    if (!sequencer.lock()->getSelectedSequence()->isUsed())
                    {
                        return;
                    }

                    openScreenById(ScreenId::SaveASequenceScreen);
                    break;
                case 2:
                    openScreenById(ScreenId::SaveApsFileScreen);
                    break;
                case 3:
                    openScreenById(ScreenId::SaveAProgramScreen);
                    break;
                case 4:
                    if (sampler.lock()->getSoundCount() == 0)
                    {
                        break;
                    }

                    openScreenById(ScreenId::SaveASoundScreen);
                    break;
                default:;
            }
        }
        default:;
    }
}

void SaveScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "type")
    {
        setType(type + i);
    }
    else if (focusedFieldName == "directory")
    {
        const auto disk = mpc.getDisk();
        const auto currentDir = disk->getDirectoryName();
        const auto parents = disk->getParentFileNames();

        int position = -1;

        for (int j = 0; j < parents.size(); j++)
        {
            if (parents[j] == currentDir)
            {
                position = j;
                break;
            }
        }

        if (const int candidate = position + i;
            position != -1 && candidate >= 0 && candidate < parents.size())
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
                else
                {
                    // From the user's perspective we stay where we are if the
                    // above moveForward call fails.
                    disk->moveForward(currentDir);
                }
            }
        }
    }
    else if (focusedFieldName == "file")
    {
        switch (type)
        {
            case 1:
                sequencer.lock()->setSelectedSequenceIndex(
                    sequencer.lock()->getSelectedSequenceIndex() + i, true);
                break;
            case 3:
            {
                unsigned char counter = 0;

                for (int idx = programIndex;
                     i < 0 ? idx >= 0 : idx < Mpc2000XlSpecs::MAX_PROGRAM_COUNT;
                     i < 0 ? idx-- : idx++)
                {
                    if (sampler.lock()->getProgram(idx)->isUsed())
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
                sampler.lock()->setSoundIndex(sampler.lock()->getSoundIndex() +
                                              i);
                break;
            default:;
        }
        displayFile();
        displaySize();
    }
    else if (focusedFieldName == "device")
    {
        if (device + i < 0 || device + i >= mpc.getDisks().size())
        {
            return;
        }

        device += i;
        displayDevice();
        displayDeviceType();
        ls.lock()->setFunctionKeysArrangement(
            mpc.getDiskController()->getActiveDiskIndex() == device ? 0 : 1);
    }
}

void SaveScreen::setType(const int i)
{
    type = std::clamp(i, 0, 4);

    if (i == 3)
    {
        for (int j = 0; j < Mpc2000XlSpecs::MAX_PROGRAM_COUNT; j++)
        {
            if (sampler.lock()->getProgram(j)->isUsed())
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

void SaveScreen::displayType() const
{
    findField("type")->setText(types[type]);
}

void SaveScreen::displayFile() const
{
    std::string fileName;

    switch (type)
    {
        case 0:
        {
            const auto saveAllFileScreen =
                mpc.screens->get<ScreenId::SaveAllFileScreen>();
            fileName = saveAllFileScreen->getFileName();
            break;
        }
        case 1:
        {
            const auto num = StrUtil::padLeft(
                std::to_string(sequencer.lock()->getSelectedSequenceIndex() +
                               1),
                "0", 2);
            const auto sequenceName =
                sequencer.lock()->getSelectedSequence()->getName();
            fileName = num + "-" + sequenceName;
            break;
        }
        case 2:
        {
            const auto saveApsFileScreen =
                mpc.screens->get<ScreenId::SaveApsFileScreen>();
            fileName = saveApsFileScreen->getFileName();
            break;
        }
        case 3:
            fileName = sampler.lock()->getProgram(programIndex)->getName();
            break;
        case 4:
            fileName = std::string(sampler.lock()->getSoundCount() == 0
                                       ? " (No sound)"
                                       : sampler.lock()->getSound()->getName());
            break;
        case 5:
            fileName = "MPC2KXL         .BIN";
            break;
        default:;
    }

    findField("file")->setText(fileName);
}

void SaveScreen::displaySize() const
{
    const auto seq = sequencer.lock()->getSelectedSequence();
    auto size = 0;

    switch (type)
    {
        case 0:
            size = sequencer.lock()->getUsedSequenceCount() * 25;
            break;
        case 1:
            size = seq->isUsed()
                       ? 10 + static_cast<int>(seq->getEventCount() * 0.001)
                       : -1;
            break;
        case 2:
            size = sampler.lock()->getProgramCount() * 4;
            break;
        case 3:
            size = 4;
            break;
        case 4:
            size = sampler.lock()->getSoundCount() == 0
                       ? -1
                       : sampler.lock()->getSound()->getSampleData()->size() *
                             2 * 0.001;
            break;
        case 5:
            size = 512;
            break;
        default:;
    }

    findLabel("size")->setText(
        StrUtil::padLeft(std::to_string(size == -1 ? 0 : size), " ", 6) + "K");
}

void SaveScreen::displayFree() const
{
    std::uintmax_t availableSpaceInBytes = 0;

    try
    {
        availableSpaceInBytes = fs::space(mpc.paths->storesPath()).available;
    }
    catch (fs::filesystem_error &)
    {
        MLOG(
            "An exception occurred when SaveScreen::displayFree was trying to "
            "query available space!");
    }

    const auto text = byte_count_to_short_string(availableSpaceInBytes);
    findLabel("free")->setText(text);
}

void SaveScreen::displayDirectory() const
{
    findField("directory")->setText(mpc.getDisk()->getDirectoryName());
}

void SaveScreen::displayDevice()
{
    const auto dev = findChild<Field>("device");
    dev->setText(mpc.getDisks()[device]->getVolume().label);
}

void SaveScreen::displayDeviceType()
{
    const auto deviceTypeLabel = findChild<Label>("device-type");
    deviceTypeLabel->setText(
        mpc.getDisks()[device]->getVolume().typeShortName());
}

void SaveScreen::up()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "device")
    {
        device = mpc.getDiskController()->getActiveDiskIndex();
        displayDevice();
        ls.lock()->setFunctionKeysArrangement(0);
    }

    ScreenComponent::up();
}

unsigned char SaveScreen::getProgramIndex() const
{
    return programIndex;
}
