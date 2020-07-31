#include "LoadASequenceFromAllScreen.hpp"

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

LoadASequenceFromAllScreen::LoadASequenceFromAllScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "load-a-sequence-from-all", layerIndex)
{
}

void LoadASequenceFromAllScreen::open()
{
	displayFile();
	displayLoadInto();
}

void LoadASequenceFromAllScreen::turnWheel(int i)
{
	init();

	if (param.compare("file") == 0)
	{
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
		loadScreen->setFileLoad(loadScreen->fileLoad  + i);
	}
	else if (param.compare("load-into") == 0)
	{
		auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(mpc.screens->getScreenComponent("load-a-sequence"));
		loadASequenceScreen->setLoadInto(loadASequenceScreen->loadInto + i);
	}
}

void LoadASequenceFromAllScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("mpc2000xl-all-file");
		break;
	case 4:
		auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(mpc.screens->getScreenComponent("load-a-sequence-from-all"));
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
		sequencer.lock()->setSequence(sequencer.lock()->getActiveSequenceIndex(), loadASequenceFromAllScreen->sequencesFromAllFile[loadScreen->fileLoad]);
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void LoadASequenceFromAllScreen::displayFile()
{
	auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(mpc.screens->getScreenComponent("load-a-sequence-from-all"));
	auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
	findField("file").lock()->setTextPadded(loadScreen->fileLoad + 1, "0");

	if (loadScreen->fileLoad >= loadASequenceFromAllScreen->sequencesFromAllFile.size())
	{
		return;
	}

	auto candidate = loadASequenceFromAllScreen->sequencesFromAllFile[loadScreen->fileLoad];
	auto name = candidate ? candidate->getName() : "(Unused)";
	findLabel("file0").lock()->setText("-" + name);
}

void LoadASequenceFromAllScreen::displayLoadInto()
{
	auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(mpc.screens->getScreenComponent("load-a-sequence"));
	findField("load-into").lock()->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
	findLabel("load-into0").lock()->setText("-" + mpc.getSequencer().lock()->getSequence(loadASequenceScreen->loadInto).lock()->getName());
}
