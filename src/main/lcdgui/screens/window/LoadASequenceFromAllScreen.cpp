#include "LoadASequenceFromAllScreen.hpp"

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

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
	sourceSeqIndex = 0;
	displayFile();
	displayLoadInto();
}

void LoadASequenceFromAllScreen::turnWheel(int i)
{
	init();

	if (param.compare("file") == 0)
	{
		setSourceSeqIndex(sourceSeqIndex + i);
	}
	else if (param.compare("load-into") == 0)
	{
		auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(mpc.screens->getScreenComponent("load-a-sequence"));
		loadASequenceScreen->setLoadInto(loadASequenceScreen->loadInto + i);
		displayLoadInto();
	}
}

void LoadASequenceFromAllScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("mpc2000xl-all-file");
		break;
	case 4:
		auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(mpc.screens->getScreenComponent("load-a-sequence-from-all"));
		auto candidate = loadASequenceFromAllScreen->sequencesFromAllFile[sourceSeqIndex];

		if (candidate)
		{
			auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(mpc.screens->getScreenComponent("load-a-sequence"));
			sequencer.lock()->setSequence(loadASequenceScreen->loadInto, candidate);
			openScreen("sequencer");
		}
		break;
	}
}

void LoadASequenceFromAllScreen::displayFile()
{
	auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(mpc.screens->getScreenComponent("load-a-sequence-from-all"));
	findField("file").lock()->setTextPadded(sourceSeqIndex + 1, "0");

	auto candidate = sourceSeqIndex < sequencesFromAllFile.size() ? loadASequenceFromAllScreen->sequencesFromAllFile[sourceSeqIndex] : shared_ptr<mpc::sequencer::Sequence>();
	auto name = candidate ? candidate->getName() : "(Unused)";
	findLabel("file0").lock()->setText("-" + name);
}

void LoadASequenceFromAllScreen::displayLoadInto()
{
	auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(mpc.screens->getScreenComponent("load-a-sequence"));
	findField("load-into").lock()->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
	findLabel("load-into0").lock()->setText("-" + mpc.getSequencer().lock()->getSequence(loadASequenceScreen->loadInto).lock()->getName());
}

void LoadASequenceFromAllScreen::setSourceSeqIndex(int i)
{
	auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(mpc.screens->getScreenComponent("load-a-sequence-from-all"));
	const auto max = loadASequenceFromAllScreen->sequencesFromAllFile.size();

	if (i < 0 || i >= max)
		return;

	sourceSeqIndex = i;
	displayFile();
}
