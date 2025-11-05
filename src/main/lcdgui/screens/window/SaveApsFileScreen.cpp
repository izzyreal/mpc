#include "SaveApsFileScreen.hpp"

#include <Util.hpp>

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/SaveAProgramScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveApsFileScreen::SaveApsFileScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-aps-file", layerIndex)
{
}

void SaveApsFileScreen::open()
{
    if (ls->isPreviousScreen({ScreenId::SaveScreen}))
    {
        fileName = "ALL_PGMS";
    }

    findField("replace-same-sounds")->setAlignment(Alignment::Centered);
    displayFile();
    displayReplaceSameSounds();
    displaySave();
}

void SaveApsFileScreen::turnWheel(int i)
{
    auto saveAProgramScreen = mpc.screens->get<ScreenId::SaveAProgramScreen>();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "save")
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

void SaveApsFileScreen::function(int i)
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
                auto replaceAction = [this, disk, apsFileName]
                {
                    auto success = disk->getFile(apsFileName)->del();

                    if (success)
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writeAps(apsFileName);
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](std::string &nameScreenName)
                    {
                        fileName = nameScreenName;
                        openScreenById(ScreenId::SaveApsFileScreen);
                    };
                    nameScreen->initialize(fileName, 16, enterAction, "save");
                };

                auto fileExistsScreen =
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
    }
}

void SaveApsFileScreen::displayFile()
{
    findField("file")->setText(fileName);
}

void SaveApsFileScreen::displaySave()
{
    auto saveAProgramScreen = mpc.screens->get<ScreenId::SaveAProgramScreen>();
    findField("save")->setText(apsSaveNames[saveAProgramScreen->save]);
}

void SaveApsFileScreen::displayReplaceSameSounds()
{
    auto saveAProgramScreen = mpc.screens->get<ScreenId::SaveAProgramScreen>();
    findField("replace-same-sounds")
        ->setText(
            std::string(saveAProgramScreen->replaceSameSounds ? "YES" : "NO"));
}

void SaveApsFileScreen::openNameScreen()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file")
    {
        const auto enterAction = [this](std::string &nameScreenName)
        {
            fileName = nameScreenName;
            openScreenById(ScreenId::SaveApsFileScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(fileName, 16, enterAction, "save-aps-file");
        openScreenById(ScreenId::NameScreen);
    }
}
