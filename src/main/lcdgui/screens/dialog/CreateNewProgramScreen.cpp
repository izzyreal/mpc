#include "CreateNewProgramScreen.hpp"

#include <controls/BaseSamplerControls.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::controls;
using namespace std;

CreateNewProgramScreen::CreateNewProgramScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "create-new-program", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void CreateNewProgramScreen::open()
{
	auto letterNumber = sampler.lock()->getProgramCount() + 21;
	newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
	
	init();

	displayNewName();
	displayMidiProgramChange();
}

void CreateNewProgramScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("program");
		break;
	case 4:
		auto newProgram = sampler.lock()->addProgram().lock();
		newProgram->setName(newName);
		newProgram->setMidiProgramChange(newProgramChange);
		mpcSoundPlayerChannel->setProgram(sampler.lock()->getProgramCount() - 1);
		ls.lock()->openScreen("program");
		break;
	}
}

void CreateNewProgramScreen::displayMidiProgramChange()
{
	findField("midi-program-change").lock()->setTextPadded(program.lock()->getMidiProgramChange(), " ");
}

void CreateNewProgramScreen::displayNewName()
{
	findField("new-name").lock()->setText(newName);
}
