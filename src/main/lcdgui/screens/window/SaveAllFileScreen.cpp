#include "SaveAllFileScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "disk/AbstractDisk.hpp"
#include "lcdgui/Label.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveAllFileScreen::SaveAllFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-all-file", layerIndex)
{
}

void SaveAllFileScreen::open()
{
    displayFile();
}
std::string SaveAllFileScreen::getFileName() const
{
    return fileName;
}

void SaveAllFileScreen::displayFile() const
{
    if (fileName.empty())
    {
        findField("file")->setText("");
        findLabel("file1")->setText("");
        return;
    }

    findField("file")->setText(fileName.substr(0, 1));
    findLabel("file1")->setText(StrUtil::padRight(fileName.substr(1), " ", 15) +
                                ".ALL");
}

void SaveAllFileScreen::openNameScreen()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file")
    {
        const auto enterAction = [this](const std::string &nameScreenName)
        {
            fileName = nameScreenName;
            openScreenById(ScreenId::SaveAllFileScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(fileName, 16, enterAction, "save-all-file");
        openScreenById(ScreenId::NameScreen);
    }
}

void SaveAllFileScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            auto allFileName = fileName + ".ALL";
            auto disk = mpc.getDisk();

            if (disk->checkExists(allFileName))
            {
                auto replaceAction = [disk, allFileName]
                {
                    if (disk->getFile(allFileName)->del())
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writeAll(allFileName);
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    const auto nameScreen =
                        mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](const std::string &nameScreenName)
                    {
                        fileName = nameScreenName;
                        openScreenById(ScreenId::SaveAllFileScreen);
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

            disk->writeAll(allFileName);
            break;
        }
        default:;
    }
}
