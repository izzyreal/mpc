#include "ProgramScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

ProgramScreen::ProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "program", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
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
		auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));
		nameScreen->setName(program.lock()->getName());
		nameScreen->parameterName = param;
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
