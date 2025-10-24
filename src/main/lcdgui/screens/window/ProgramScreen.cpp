#include "ProgramScreen.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

ProgramScreen::ProgramScreen(mpc::Mpc& mpc, const int layerIndex)
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

        const auto enterAction = [this, program](std::string& nameScreenName) {
            program->setName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<ProgramScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(program->getName(), 16, enterAction, "program");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
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
        mpc.getLayeredScreen()->openScreen<DeleteProgramScreen>();
		break;
	case 2:
	{
		if (sampler->getProgramCount() == 24)
		{
			ls->showPopupAndAwaitInteraction("Prog. directory full(24 max)");
			return;
		}
		
        mpc.getLayeredScreen()->openScreen<CreateNewProgramScreen>();
		break;
	}
	case 4:
        mpc.getLayeredScreen()->openScreen<CopyProgramScreen>();
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
	findField("midiprogramchange")->setTextPadded(program->getMidiProgramChange(), " ");
}
