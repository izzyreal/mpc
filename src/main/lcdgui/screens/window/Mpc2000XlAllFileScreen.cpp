#include "Mpc2000XlAllFileScreen.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/LoadASequenceFromAllScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

namespace
{
    bool isMpc3000AllFile(const std::shared_ptr<mpc::disk::MpcFile>& file)
    {
        if (!file)
        {
            return false;
        }

        const auto bytes = file->getBytes();
        return bytes.size() >= 2 &&
            static_cast<unsigned char>(bytes[0]) == 0x04 &&
            static_cast<unsigned char>(bytes[1]) == 0x03;
    }
}

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::open()
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    const auto bgName = isMpc3000AllFile(loadScreen->getSelectedFile())
        ? "mpc3000-all-file"
        : "mpc2000xl-all-file";
    findBackground()->setBackgroundName(bgName);
}

void Mpc2000XlAllFileScreen::function(const int i)
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();

    switch (i)
    {
        case 2:
        {
            auto result = mpc.getDisk()->readSequenceMetaInfosFromAllFile(
                loadScreen->getSelectedFile());

            if (result.has_value())
            {
                const auto loadASequenceFromAllScreen =
                    mpc.screens->get<ScreenId::LoadASequenceFromAllScreen>();

                loadASequenceFromAllScreen->sequenceMetaInfos = result.value();

                openScreenById(ScreenId::LoadASequenceFromAllScreen);
            }

            break;
        }
        case 3:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 4:
        {
            const auto selectedFile = loadScreen->getSelectedFile();
            mpc.screens->get<ScreenId::SaveAllFileScreen>()->setFileName(
                selectedFile->getNameWithoutExtension());
            auto on_success = [&, selectedFile]
            {
                openScreenById(ScreenId::LoadScreen);
            };
            mpc.getDisk()->readAll2(selectedFile, on_success);
            break;
        }
        default:;
    }
}
