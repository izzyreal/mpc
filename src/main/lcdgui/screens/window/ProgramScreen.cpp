#include "ProgramScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

ProgramScreen::ProgramScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "program", layerIndex)
{
}

void ProgramScreen::open()
{
    displayProgramName();
    displayMidiProgramChange();
}

void ProgramScreen::openNameScreen()
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "programname")
    {
        auto program = getProgramOrThrow();

        const auto enterAction = [this, program](std::string &nameScreenName)
        {
            program->setName(nameScreenName);
            openScreenById(ScreenId::ProgramScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        nameScreen->initialize(program->getName(), 16, enterAction, "program");
        openScreenById(ScreenId::NameScreen);
    }
}

void ProgramScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "midiprogramchange")
    {
        auto program = getProgramOrThrow();
        program->setMidiProgramChange(program->getMidiProgramChange() + i);
        displayMidiProgramChange();
    }
}

void ProgramScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            openScreenById(ScreenId::DeleteProgramScreen);
            break;
        case 2:
        {
            if (sampler->getProgramCount() == 24)
            {
                ls->showPopupAndAwaitInteraction(
                    "Prog. directory full(24 max)");
                return;
            }

            openScreenById(ScreenId::CreateNewProgramScreen);
            break;
        }
        case 4:
            openScreenById(ScreenId::CopyProgramScreen);
            break;
    }
}

void ProgramScreen::displayProgramName()
{
    auto program = getProgramOrThrow();
    findField("programname")->setText(program->getName());
}

void ProgramScreen::displayMidiProgramChange()
{
    auto program = getProgramOrThrow();
    findField("midiprogramchange")
        ->setTextPadded(program->getMidiProgramChange(), " ");
}
