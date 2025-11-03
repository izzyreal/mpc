#include "LoadASequenceFromAllScreen.hpp"

#include "lcdgui/Label.hpp"
#include "sequencer/Sequence.hpp"

#include "lcdgui/screens/window/LoadASequenceScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASequenceFromAllScreen::LoadASequenceFromAllScreen(mpc::Mpc &mpc,
                                                       const int layerIndex)
    : ScreenComponent(mpc, "load-a-sequence-from-all", layerIndex)
{
}

void LoadASequenceFromAllScreen::open()
{
    sourceSeqIndex = 0;
    displayFile();
    displayLoadInto();
}

void LoadASequenceFromAllScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file")
    {
        setSourceSeqIndex(sourceSeqIndex + i);
    }
    else if (focusedFieldName == "load-into")
    {
        auto loadASequenceScreen =
            mpc.screens->get<ScreenId::LoadASequenceScreen>();
        loadASequenceScreen->setLoadInto(loadASequenceScreen->loadInto + i);
        displayLoadInto();
    }
}

void LoadASequenceFromAllScreen::function(int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::Mpc2000XlAllFileScreen);
            break;
        case 4:
            auto candidate = sequencesFromAllFile[sourceSeqIndex];

            if (candidate)
            {
                auto loadASequenceScreen =
                    mpc.screens->get<ScreenId::LoadASequenceScreen>();
                sequencer->setSequence(loadASequenceScreen->loadInto,
                                       candidate);
                openScreenById(ScreenId::LoadScreen);
            }
            break;
    }
}

void LoadASequenceFromAllScreen::displayFile()
{
    if (sourceSeqIndex >= sequencesFromAllFile.size())
    {
        return;
    }

    findField("file")->setTextPadded(sourceSeqIndex + 1, "0");

    auto candidate = sequencesFromAllFile[sourceSeqIndex];

    auto name = candidate ? candidate->getName() : "(Unused)";

    findLabel("file0")->setText("-" + name);
}

void LoadASequenceFromAllScreen::displayLoadInto()
{
    auto loadASequenceScreen =
        mpc.screens->get<ScreenId::LoadASequenceScreen>();
    findField("load-into")
        ->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
    findLabel("load-into0")
        ->setText(
            "-" +
            sequencer->getSequence(loadASequenceScreen->loadInto)->getName());
}

void LoadASequenceFromAllScreen::setSourceSeqIndex(int i)
{
    if (i < 0 || i >= sequencesFromAllFile.size())
    {
        return;
    }

    sourceSeqIndex = i;
    displayFile();
}
