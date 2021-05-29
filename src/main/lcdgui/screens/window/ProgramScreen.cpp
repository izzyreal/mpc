#include "ProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::controls;
using namespace std;

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

void ProgramScreen::turnWheel(int i)
{
    init();

	if (param.compare("programname") == 0)
	{
        auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto _program = program.lock();
        nameScreen->setName(_program->getName());

        auto renamer = [_program](string& newName) {
            _program->setName(newName);
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "program");
        openScreen("name");
    }
	else if (param.compare("midiprogramchange") == 0)
	{
		program.lock()->setMidiProgramChange(program.lock()->getMidiProgramChange() + i);
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
		if (sampler.lock()->getProgramCount() == 24)
		{
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");
			popupScreen->setText("Prog. directory full(24 max)");
			popupScreen->returnToScreenAfterInteraction(name);
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
	findField("programname").lock()->setText(program.lock()->getName());
}

void ProgramScreen::displayMidiProgramChange()
{
	findField("midiprogramchange").lock()->setTextPadded(program.lock()->getMidiProgramChange(), " ");
}
