#include "SaveAProgramScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/screens/SaveScreen.hpp"

#include "Util.hpp"
#include "disk/AbstractDisk.hpp"
#include "lcdgui/Label.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveAProgramScreen::SaveAProgramScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-a-program", layerIndex)
{
}

void SaveAProgramScreen::open()
{
    if (ls->isPreviousScreen({ScreenId::SaveScreen}))
    {
        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        const auto saveScreen = mpc.screens->get<ScreenId::SaveScreen>();
        nameScreen->setNameToEdit(
            sampler->getProgram(saveScreen->getProgramIndex())->getName());
    }

    findField("replace-same-sounds")->setAlignment(Alignment::Centered);
    displayFile();
    displaySave();
    displayReplaceSameSounds();
}

void SaveAProgramScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "save")
    {
        setSave(save + i);
    }
    else if (focusedFieldName == "replace-same-sounds")
    {
        replaceSameSounds = i > 0;
        displayReplaceSameSounds();
    }
}

void SaveAProgramScreen::function(const int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
            auto fileName =
                Util::getFileName(nameScreen->getNameWithoutSpaces()) + ".PGM";
            auto disk = mpc.getDisk();

            auto program = getProgramOrThrow();

            if (disk->checkExists(fileName))
            {
                auto replaceAction = [disk, fileName, program]
                {
                    const auto success = disk->getFile(fileName)->del();

                    if (success)
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writePgm(program, fileName);
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    const auto nameScreen =
                        mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](std::string &)
                    {
                        openScreenById(ScreenId::SaveAProgramScreen);
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
                break;
            }

            disk->writePgm(program, fileName);
            break;
        }
    }
}

void SaveAProgramScreen::setSave(const int i)
{
    save = std::clamp(i, 0, 2);
    displaySave();
}

void SaveAProgramScreen::displaySave() const
{
    findField("save")->setText(pgmSaveNames[save]);
}

void SaveAProgramScreen::displayReplaceSameSounds() const
{
    findField("replace-same-sounds")
        ->setText(std::string(replaceSameSounds ? "YES" : "NO"));
}

void SaveAProgramScreen::displayFile() const
{
    const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    findLabel("file")->setText(nameScreen->getNameWithoutSpaces() + ".PGM");
}
