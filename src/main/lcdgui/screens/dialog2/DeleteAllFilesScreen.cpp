#include "DeleteAllFilesScreen.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"

using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

DeleteAllFilesScreen::DeleteAllFilesScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-all-files", layerIndex)
{
}

void DeleteAllFilesScreen::open()
{
    displayDelete();
}

void DeleteAllFilesScreen::displayDelete()
{
    findField("delete")->setText(views[delete_]);
}

void DeleteAllFilesScreen::setDelete(int i)
{
    delete_ = std::clamp(i, 0, 8);
    displayDelete();
}

void DeleteAllFilesScreen::turnWheel(int i)
{
    setDelete(delete_ + i);
}

void DeleteAllFilesScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::DeleteFileScreen);
            break;
        case 4:
        {
            const auto success = mpc.getDisk()->deleteAllFiles(delete_);

            if (success)
            {
                const auto loadScreen =
                    mpc.screens->get<ScreenId::LoadScreen>();
                const auto directoryScreen =
                    mpc.screens->get<ScreenId::DirectoryScreen>();
                loadScreen->fileLoad = 0;
                directoryScreen->yOffset1 = 0;
                mpc.getDisk()->initFiles();
            }

            openScreenById(ScreenId::DirectoryScreen);
            break;
        }
    }
}
