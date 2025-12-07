#include "LoadASequenceFromAllScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequence.hpp"

#include "lcdgui/screens/window/LoadASequenceScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadASequenceFromAllScreen::LoadASequenceFromAllScreen(Mpc &mpc,
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

void LoadASequenceFromAllScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "file")
    {
        setSourceSeqIndex(sourceSeqIndex + i);
    }
    else if (focusedFieldName == "load-into")
    {
        const auto loadASequenceScreen =
            mpc.screens->get<ScreenId::LoadASequenceScreen>();
        loadASequenceScreen->setLoadInto(loadASequenceScreen->loadInto + i);
        displayLoadInto();
    }
}

void LoadASequenceFromAllScreen::function(const int i)
{
    if (i == 3)
    {
        openScreenById(ScreenId::Mpc2000XlAllFileScreen);
    }
    else if (i == 4)
    {
        if (const auto candidate = sequencesFromAllFile[sourceSeqIndex])
        {
            const auto loadASequenceScreen =
                mpc.screens->get<ScreenId::LoadASequenceScreen>();

            sequencer.lock()->copySequence(candidate,
                                           loadASequenceScreen->loadInto);

            openScreenById(ScreenId::LoadScreen);
        }
    }
}

void LoadASequenceFromAllScreen::displayFile() const
{
    if (sourceSeqIndex >= sequencesFromAllFile.size())
    {
        return;
    }

    findField("file")->setTextPadded(sourceSeqIndex + 1, "0");

    const auto candidate = sequencesFromAllFile[sourceSeqIndex];

    const auto name = candidate ? candidate->getName() : "(Unused)";

    findLabel("file0")->setText("-" + name);
}

void LoadASequenceFromAllScreen::displayLoadInto() const
{
    const auto loadASequenceScreen =
        mpc.screens->get<ScreenId::LoadASequenceScreen>();
    findField("load-into")
        ->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
    findLabel("load-into0")
        ->setText("-" + sequencer.lock()
                            ->getSequence(loadASequenceScreen->loadInto)
                            ->getName());
}

void LoadASequenceFromAllScreen::setSourceSeqIndex(const int i)
{
    if (i < 0 || i >= sequencesFromAllFile.size())
    {
        return;
    }

    sourceSeqIndex = i;
    displayFile();
}
