#include "CreateNewProgramScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

CreateNewProgramScreen::CreateNewProgramScreen(Mpc &mpc,
                                               const int layerIndex)
    : ScreenComponent(mpc, "create-new-program", layerIndex)
{
}

void CreateNewProgramScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "midi-program-change")
    {
        setMidiProgramChange(midiProgramChange + i);
    }
}

void CreateNewProgramScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "new-name")
    {
        const auto enterAction = [this](std::string &nameScreenName)
        {
            newName = nameScreenName;
            openScreenById(ScreenId::CreateNewProgramScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(newName, 16, enterAction, "create-new-program");
        openScreenById(ScreenId::NameScreen);
    }
}

void CreateNewProgramScreen::open()
{
    if (ls->isPreviousScreenNot({ScreenId::NameScreen}))
    {
        auto letterIndex = 21 + 24;

        for (int i = 0; i < sampler->getPrograms().size(); i++)
        {
            if (!sampler->getProgram(i))
            {
                letterIndex = 21 + i;
                midiProgramChange = i + 1;
                break;
            }
        }

        newName = "NewPgm-" + Mpc::akaiAscii[letterIndex];
    }

    displayNewName();
    displayMidiProgramChange();
}

void CreateNewProgramScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::ProgramScreen);
            break;
        case 4:
            const auto newProgram =
                sampler->createNewProgramAddFirstAvailableSlot().lock();
            newProgram->setName(newName);
            newProgram->setMidiProgramChange(midiProgramChange);

            auto index = sampler->getProgramCount() - 1;

            for (int j = 0; j < sampler->getPrograms().size(); j++)
            {
                if (sampler->getProgram(j) == newProgram)
                {
                    index = j;
                    break;
                }
            }

            getActiveDrumBus()->setProgram(index);
            openScreenById(ScreenId::ProgramScreen);
            break;
    }
}

void CreateNewProgramScreen::displayMidiProgramChange()
{
    findField("midi-program-change")->setTextPadded(midiProgramChange);
}

void CreateNewProgramScreen::displayNewName()
{
    findField("new-name")->setText(newName);
}

void CreateNewProgramScreen::setMidiProgramChange(int i)
{
    if (i < 1)
    {
        i = 1;
    }
    else if (i > 128)
    {
        i = 128;
    }

    if (midiProgramChange == i)
    {
        return;
    }

    midiProgramChange = i;
    displayMidiProgramChange();
}
