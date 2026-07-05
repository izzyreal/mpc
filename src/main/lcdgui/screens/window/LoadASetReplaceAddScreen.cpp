#include "LoadASetReplaceAddScreen.hpp"

#include "Mpc.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/Mpc60SetProgramLoader.hpp"
#include "file/kaitai/Mpc60SetPreview.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASetScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "utils/SimpleAction.hpp"

#include <memory>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASetReplaceAddScreen::LoadASetReplaceAddScreen(Mpc &mpc,
                                                   const int layerIndex)
    : ScreenComponent(mpc, "load-a-set-replace-add", layerIndex)
{
}

LoadASetReplaceAddScreen::~LoadASetReplaceAddScreen()
{
    if (loadThread.joinable())
    {
        loadThread.join();
    }
}

void LoadASetReplaceAddScreen::open()
{
    displayLoadReplaceSound();
}

void LoadASetReplaceAddScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "load-replace-sound")
    {
        loadReplaceSameSound = i > 0;
        displayLoadReplaceSound();
    }
}

void LoadASetReplaceAddScreen::function(const int i)
{
    switch (i)
    {
        case 2:
        case 4:
        {
            const auto loadASetScreen = mpc.screens->get<ScreenId::LoadASetScreen>();
            const auto preview = loadASetScreen->getPreview();
            const auto file = mpc.screens->get<ScreenId::LoadScreen>()->getSelectedFile();

            if (preview == nullptr || file == nullptr)
            {
                ls.lock()->showPopupAndThenOpen(ScreenId::LoadScreen,
                                                "Can't read SET", 1000);
                break;
            }

            const bool clearExisting = i == 2;
            auto previewCopy = std::make_shared<file::kaitai::Mpc60SetPreview>(*preview);
            auto conversionTableCopy =
                std::make_shared<file::kaitai::Mpc60SetProgramLoader::ConversionTable>(
                    loadASetScreen->getConversionTable());
            const auto layeredScreen = mpc.getLayeredScreen();

            layeredScreen->showPopup("Loading " + file->getName());

            if (loadThread.joinable())
            {
                loadThread.join();
            }

            loadThread = std::thread(
                [this, file, previewCopy, conversionTableCopy, clearExisting,
                 layeredScreen]
                {
                    const auto loaded =
                        file::kaitai::Mpc60SetProgramLoader::load(
                            mpc, file, *previewCopy, *conversionTableCopy,
                            clearExisting);

                    if (!loaded)
                    {
                        layeredScreen->postToUiThread(mpc::utils::Task(
                            [layeredScreen]
                            {
                                layeredScreen->showPopupAndThenOpen(
                                    ScreenId::LoadScreen, "Can't read SET",
                                    1000);
                            }));
                        return;
                    }

                    mpc.getPerformanceManager().lock()->enqueueCallback(
                        mpc::utils::SimpleAction(
                            [layeredScreen]
                        {
                            layeredScreen->postToUiThread(mpc::utils::Task(
                                [layeredScreen]
                                {
                                    layeredScreen->openScreenById(
                                        ScreenId::LoadScreen);
                                }));
                        }));
                });
            break;
        }
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        default:;
    }
}

void LoadASetReplaceAddScreen::displayLoadReplaceSound() const
{
    findField("load-replace-sound")
        ->setText(std::string(loadReplaceSameSound ? "YES" : "NO(FASTER)"));
}
