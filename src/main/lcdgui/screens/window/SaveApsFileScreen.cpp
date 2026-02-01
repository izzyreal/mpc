#include "SaveApsFileScreen.hpp"

#include "Util.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/SaveAProgramScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveApsFileScreen::SaveApsFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-aps-file", layerIndex)
{
}

void SaveApsFileScreen::open()
{
    findField("replace-same-sounds")->setAlignment(Alignment::Centered);
    displayFile();
    displayReplaceSameSounds();
    displaySave();
}

std::string SaveApsFileScreen::getFileName() const
{
    return fileName;
}

void SaveApsFileScreen::setFileName(const std::string &fileNameToUse)
{
    fileName = fileNameToUse;
}

void SaveApsFileScreen::turnWheel(const int i)
{
    const auto saveAProgramScreen =
        mpc.screens->get<ScreenId::SaveAProgramScreen>();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "save")
    {
        saveAProgramScreen->setSave(saveAProgramScreen->save + i);
        displaySave();
    }
    else if (focusedFieldName == "replace-same-sounds")
    {
        saveAProgramScreen->replaceSameSounds = i > 0;
        displayReplaceSameSounds();
    }
}

void SaveApsFileScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
            std::string apsFileName = fileName + ".APS";

            auto disk = mpc.getDisk();

            if (disk->checkExists(apsFileName))
            {
                auto replaceAction = [disk, apsFileName]
                {
                    if (disk->getFile(apsFileName)->del())
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writeAps(apsFileName);
                    }
                };

                const auto initializeNameScreen = [this, nameScreen]
                {
                    auto enterAction = [this](const std::string &nameScreenName)
                    {
                        fileName = nameScreenName;
                        openScreenById(ScreenId::SaveApsFileScreen);
                    };
                    nameScreen->initialize(fileName, 16, enterAction, "save");
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

            disk->writeAps(apsFileName);
            break;
        }
        default:;
    }
}

void SaveApsFileScreen::displayFile() const
{
    findField("file")->setText(fileName);
}

void SaveApsFileScreen::displaySave() const
{
    const auto saveAProgramScreen =
        mpc.screens->get<ScreenId::SaveAProgramScreen>();
    findField("save")->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds() const
{
    const auto saveAProgramScreen =
        mpc.screens->get<ScreenId::SaveAProgramScreen>();
    findField("replace-same-sounds")
        ->setText(
            std::string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}

void SaveApsFileScreen::openNameScreen()
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "file")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            fileName = nameScreenName;
            openScreenById(ScreenId::SaveApsFileScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(fileName, 16, enterAction, "save-aps-file");
        openScreenById(ScreenId::NameScreen);
    }
}
