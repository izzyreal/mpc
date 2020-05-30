#include "ProgramScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

ProgramScreen::ProgramScreen(const int layerIndex) 
	: ScreenComponent("program", layerIndex)
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
        nameGui->setName(program.lock()->getName());
        nameGui->setParameterName(param);
        ls.lock()->openScreen("name");
    }
	else if (param.compare("midiprogramchange") == 0)
	{
		program.lock()->setMidiProgramChange(program.lock()->getMidiProgramChange() + i);
	}
}

void ProgramScreen::function(int i)
{
	BaseControls::function(i);

	switch (i)
	{
	case 1:
		ls.lock()->openScreen("delete-program");
		break;
	case 2:
	{
		ls.lock()->openScreen("create-new-program");
		break;
	}
	case 4:
		ls.lock()->openScreen("copy-program");
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
