#include "DeleteFileScreen.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include <thread>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;

DeleteFileScreen::DeleteFileScreen(mpc::Mpc &mpc, int layerIndex)
    : ScreenComponent(mpc, "delete-file", layerIndex)
{
}

void DeleteFileScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            mpc.getLayeredScreen()->openScreen<DeleteAllFilesScreen>();
            break;

        case 4:
        {
            auto directoryScreen = mpc.screens->get<DirectoryScreen>();
            ls->showPopupAndAwaitInteraction(
                "Delete: " + directoryScreen->getSelectedFile()->getName());
            std::thread(
                [this]
                {
                    deleteFile();
                })
                .detach();
            break;
        }
    }
}

void DeleteFileScreen::deleteFile()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto disk = mpc.getDisk();

    if (disk->deleteSelectedFile())
    {
        disk->flush();
        disk->initFiles();

        auto loadScreen = mpc.screens->get<LoadScreen>();
        loadScreen->setFileLoad(loadScreen->fileLoad - 1);

        auto directoryScreen = mpc.screens->get<DirectoryScreen>();
        directoryScreen->setYOffset1(directoryScreen->yOffset1 - 1);
    }

    mpc.getLayeredScreen()->openScreen<DirectoryScreen>();
}
