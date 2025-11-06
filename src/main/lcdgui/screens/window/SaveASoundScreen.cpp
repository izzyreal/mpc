#include "SaveASoundScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "sampler/Sampler.hpp"
#include "Util.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

SaveASoundScreen::SaveASoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-a-sound", layerIndex)
{
}

void SaveASoundScreen::open()
{
    if (ls->isPreviousScreen({ScreenId::SaveScreen}))
    {
        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->setNameToEdit(sampler->getSound()->getName());
    }

    displayFile();
    displayFileType();
}

void SaveASoundScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file" && i > 0)
    {
        sampler->selectPreviousSound();
        displayFile();
    }
    else if (focusedFieldName == "file" && i < 0)
    {
        sampler->selectNextSound();
        displayFile();
    }
    else if (focusedFieldName == "file-type")
    {
        setFileType(fileType + i);
    }
}

void SaveASoundScreen::function(const int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            auto disk = mpc.getDisk();
            auto s = sampler->getSound();
            const auto ext = std::string(fileType == 0 ? ".SND" : ".WAV");
            auto fileName =
                Util::getFileName(mpc.screens->get<ScreenId::NameScreen>()
                                      ->getNameWithoutSpaces()) +
                ext;

            auto saveAction = [this, disk, s, fileName]
            {
                disk->flush();
                disk->initFiles();

                if (fileType == 0)
                {
                    disk->writeSnd(s, fileName);
                }
                else
                {
                    disk->writeWav(s, fileName);
                }

                disk->flush();

                ls->showPopupAndThenReturnToLayer("Saving " + fileName, 700, 0);
            };

            if (disk->checkExists(fileName))
            {
                auto replaceAction = [saveAction, disk, fileName]
                {
                    const auto success = disk->getFile(fileName)->del();

                    if (success)
                    {
                        saveAction();
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    const auto nameScreen =
                        mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](std::string &)
                    {
                        openScreenById(ScreenId::SaveASoundScreen);
                    };
                    nameScreen->initialize(nameScreen->getNameWithoutSpaces(),
                                           16, enterAction, "save");
                };

                const auto fileExistsScreen =
                    mpc.screens->get<ScreenId::FileExistsScreen>();
                fileExistsScreen->initialize(
                    replaceAction, initializeNameScreen,
                    [this]
                    {
                        openScreenById(ScreenId::SaveScreen);
                    });
                openScreenById(ScreenId::FileExistsScreen);
                return;
            }
            saveAction();
            break;
        }
    }
}

void SaveASoundScreen::setFileType(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    fileType = i;
    displayFileType();
}

void SaveASoundScreen::displayFileType() const
{
    findField("file-type")
        ->setText(std::string(fileType == 0 ? "MPC2000" : "WAV"));
}

void SaveASoundScreen::displayFile() const
{
    findField("file")->setText(
        mpc.screens->get<ScreenId::NameScreen>()->getNameWithoutSpaces());
}
