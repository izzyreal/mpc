#include "DeleteSequenceScreen.hpp"

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
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
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		displaySequenceNumberName();
	}
}

void DeleteSequenceScreen::function(int i)
{
	init();
	
	switch (i) {
	case 2:
		ls.lock()->openScreen("delete-all-sequences");
		break;
	case 3:
		ls.lock()->openScreen("sequence");
		break;
	case 4:
		sequencer.lock()->purgeSequence(sequencer.lock()->getActiveSequenceIndex());
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void DeleteSequenceScreen::displaySequenceNumberName()
{
	auto sequenceName = sequencer.lock()->getActiveSequence().lock()->getName();
	findField("sq").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), " ", 2) + "-" + sequenceName);
}
