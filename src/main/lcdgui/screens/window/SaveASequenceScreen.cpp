#include "SaveASequenceScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"

#include "Util.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;

SaveASequenceScreen::SaveASequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "save-a-sequence", layerIndex)
{
}

void SaveASequenceScreen::open()
{
    if (ls.lock()->isPreviousScreen({ScreenId::SaveScreen}) || fileName.empty())
    {
        fileName = sequencer.lock()->getSelectedSequence()->getName();
    }

    displaySaveAs();
    displayFile();
}

void SaveASequenceScreen::turnWheel(const int i)
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
            fileName, 16,
            [this](std::string &nameScreenName)
            {
                fileName = nameScreenName;
                openScreenById(ScreenId::SaveASequenceScreen);
            },
            "save-a-sequence");
        openScreenById(ScreenId::NameScreen);
    }
}

void SaveASequenceScreen::function(const int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SaveScreen);
            break;
        case 4:
        {
            auto outputFileName =
                Util::getFileName(this->fileName) + ".MID";
            auto disk = mpc.getDisk();

            if (disk->checkExists(outputFileName))
            {
                auto replaceAction = [this, disk, outputFileName]
                {
                    const auto success = disk->deleteFileOrOpenErrorPopup(
                        disk->getFile(outputFileName));

                    if (success)
                    {
                        disk->flush();
                        disk->initFiles();
                        disk->writeMid(sequencer.lock()->getSelectedSequence(),
                                       outputFileName);
                    }
                };

                const auto initializeNameScreen = [this]
                {
                    const auto nameScreen =
                        mpc.screens->get<ScreenId::NameScreen>();
                    auto enterAction = [this](std::string &nameScreenName)
                    {
                        fileName = nameScreenName;
                        openScreenById(ScreenId::SaveASequenceScreen);
                    };
                    nameScreen->initialize(fileName,
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
                return;
            }

            const auto seq = sequencer.lock()->getSelectedSequence();

            disk->writeMid(seq, outputFileName);
            break;
        }
    }
}

void SaveASequenceScreen::displaySaveAs() const
{
    findField("saveas")->setText("MIDI FILE TYPE " +
                                 std::to_string(saveSequenceAs));
}

void SaveASequenceScreen::displayFile() const
{
    const auto name = Util::getFileName(fileName);

    if (name.length() < 2)
    {
        return;
    }

    findField("file")->setText(name.substr(0, 1));
    findLabel("file1")->setText(name.substr(1));
}

void SaveASequenceScreen::setSaveSequenceAs(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    saveSequenceAs = i;
    displaySaveAs();
}
