#include "SaveASequenceScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

#include <Util.hpp>
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveASequenceScreen::SaveASequenceScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-a-sequence", layerIndex)
{
}

void SaveASequenceScreen::open()
{
    if (ls->isPreviousScreen<ScreenId::SaveScreen>())
    {
        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->setNameToEdit(sequencer->getActiveSequence()->getName());
    }

    displaySaveAs();
    displayFile();
}

void SaveASequenceScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "saveas")
    {
        setSaveSequenceAs(saveSequenceAs + i);
    }
}

void SaveASequenceScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file")
    {
        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(
            sequencer->getActiveSequence()->getName(), 16,
            [this](std::string &)
            {
                openScreenById(ScreenId::SaveASequenceScreen);
            },
            "save-a-sequence");
        openScreenById(ScreenId::NameScreen);
    }
}

void SaveASequenceScreen::function(int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
            auto fileName =
                mpc::Util::getFileName(nameScreen->getNameWithoutSpaces()) +
                ".MID";
            auto disk = mpc.getDisk();

            if (disk->checkExists(fileName))
            {
                auto replaceAction = [this, disk, fileName]
                {
                    auto success = disk->getFile(fileName)->del();

                    if (success)
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writeMid(sequencer->getActiveSequence(),
                                       fileName);
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](std::string &)
                    {
                        openScreenById(ScreenId::SaveASequenceScreen);
                    };
                    nameScreen->initialize(nameScreen->getNameWithoutSpaces(),
                                           16, enterAction, "save");
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

            auto seq = sequencer->getActiveSequence();

            disk->writeMid(seq, fileName);
            break;
        }
    }
}

void SaveASequenceScreen::displaySaveAs()
{
    findField("saveas")->setText("MIDI FILE TYPE " +
                                 std::to_string(saveSequenceAs));
}

void SaveASequenceScreen::displayFile()
{
    auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    auto name = nameScreen->getNameWithoutSpaces();

    if (name.length() < 2)
    {
        return;
    }

    findField("file")->setText(name.substr(0, 1));
    findLabel("file1")->setText(name.substr(1));
}

void SaveASequenceScreen::setSaveSequenceAs(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    saveSequenceAs = i;
    displaySaveAs();
}
