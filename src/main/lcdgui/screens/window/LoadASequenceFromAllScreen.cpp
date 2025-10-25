#include "LoadASequenceFromAllScreen.hpp"

#include "sequencer/Sequence.hpp"

#include "lcdgui/screens/window/LoadASequenceScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASequenceFromAllScreen::LoadASequenceFromAllScreen(mpc::Mpc &mpc, const int layerIndex)
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
        auto loadASequenceScreen = mpc.screens->get<LoadASequenceScreen>();
        loadASequenceScreen->setLoadInto(loadASequenceScreen->loadInto + i);
        displayLoadInto();
    }
}

void LoadASequenceFromAllScreen::function(int i)
{

    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->openScreen<Mpc2000XlAllFileScreen>();
            break;
        case 4:
            auto candidate = sequencesFromAllFile[sourceSeqIndex];

            if (candidate)
            {
                auto loadASequenceScreen = mpc.screens->get<LoadASequenceScreen>();
                sequencer.lock()->setSequence(loadASequenceScreen->loadInto, candidate);
                mpc.getLayeredScreen()->openScreen<LoadScreen>();
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
    auto loadASequenceScreen = mpc.screens->get<LoadASequenceScreen>();
    findField("load-into")->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
    findLabel("load-into0")->setText("-" + sequencer.lock()->getSequence(loadASequenceScreen->loadInto)->getName());
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
