#include "DeleteFolderScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"

#ifdef __linux__
#include <pthread.hpp>
#endif // __linux__

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

DeleteFolderScreen::DeleteFolderScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "delete-folder", layerIndex)
{
}

void DeleteFolderScreen::static_deleteFolder(void *this_p)
{
    static_cast<DeleteFolderScreen *>(this_p)->deleteFolder();
}

void DeleteFolderScreen::deleteFolder()
{
    const auto directoryScreen = mpc.screens->get<ScreenId::DirectoryScreen>();
    openScreenById(ScreenId::PopupScreen);
    const auto popupScreen = mpc.screens->get<ScreenId::PopupScreen>();
    const auto file = directoryScreen->getSelectedFile();
    const auto fileName = file->getName();
    popupScreen->setText("Delete:" + fileName);

    const auto disk = mpc.getDisk();
    auto parentFileNames = disk->getParentFileNames();

    if (disk->deleteRecursive(file))
    {
        auto currentIndex = directoryScreen->yPos0 + directoryScreen->yOffset0;

        disk->flush();
        disk->moveBack();
        disk->initFiles();

        for (int i = 0; i < parentFileNames.size(); i++)
        {
            if (parentFileNames[i] == fileName)
            {
                parentFileNames.erase(begin(parentFileNames) + i);
                break;
            }
        }

        if (currentIndex >= parentFileNames.size() && currentIndex != 0)
        {
            currentIndex--;

            if (directoryScreen->yPos0 == 0)
            {
                directoryScreen->yOffset0 -= 1;
            }
            else
            {
                directoryScreen->yPos0 -= 1;
            }
        }

        if (parentFileNames.size() == 0)
        {
            directoryScreen->yPos0 = 0;
            directoryScreen->yOffset0 = 0;
            disk->moveBack();
            disk->initFiles();
        }
        else
        {
            const auto nextDir = parentFileNames[currentIndex];
            disk->moveForward(nextDir);
            disk->initFiles();
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    openScreenById(ScreenId::DirectoryScreen);
}

void DeleteFolderScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 4:
            if (deleteFolderThread.joinable())
            {
                deleteFolderThread.join();
            }

            deleteFolderThread =
                std::thread(&DeleteFolderScreen::static_deleteFolder, this);
            break;
    }
}

DeleteFolderScreen::~DeleteFolderScreen()
{
    if (deleteFolderThread.joinable())
    {
        deleteFolderThread.join();
    }
}
