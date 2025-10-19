#include "ProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

ProgramScreen::ProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "program", layerIndex)
{
}

void ProgramScreen::open()
{
	init();
	displayProgramName();
	displayMidiProgramChange();
}

void ProgramScreen::openNameScreen()
{
    init();

    if (param == "programname")
    {
        const auto enterAction = [this](std::string& nameScreenName) {
            program->setName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(program->getName(), 16, enterAction, name);
        openScreen("name");
    }
}

void ProgramScreen::turnWheel(int i)
{
    init();

	if (param == "midiprogramchange")
	{
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
		openScreen("delete-program");
		break;
	case 2:
	{
		if (sampler->getProgramCount() == 24)
		{
			auto popupScreen = mpc.screens->get<PopupScreen>();
			popupScreen->setText("Prog. directory full(24 max)");
			popupScreen->setScreenToReturnTo(name);
			openScreen("popup");
			return;
		}
		
		openScreen("create-new-program");
		break;
	}
	case 4:
		openScreen("copy-program");
		break;
	}
}

void ProgramScreen::displayProgramName()
{
	findField("programname")->setText(program->getName());
}

void ProgramScreen::displayMidiProgramChange()
{
	findField("midiprogramchange")->setTextPadded(program->getMidiProgramChange(), " ");
}
