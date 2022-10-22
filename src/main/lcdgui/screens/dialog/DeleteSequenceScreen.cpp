#include "DeleteSequenceScreen.hpp"

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

DeleteSequenceScreen::DeleteSequenceScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-sequence", layerIndex)
{
}

void DeleteSequenceScreen::open()
{
	displaySequenceNumberName();
}

void DeleteSequenceScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0)
	{
		sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() + i);
		displaySequenceNumberName();
	}
}

void DeleteSequenceScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 2:
		openScreen("delete-all-sequences");
		break;
	case 3:
		openScreen("sequence");
		break;
	case 4:
		sequencer->move(0);
		sequencer->purgeSequence(sequencer->getActiveSequenceIndex());
		openScreen("sequencer");
		break;
	}
}

void DeleteSequenceScreen::displaySequenceNumberName()
{
	auto sequenceName = sequencer->getActiveSequence()->getName();
	findField("sq").lock()->setText(StrUtil::padLeft(to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + sequenceName);
}
