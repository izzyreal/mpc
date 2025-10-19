#include "DeleteSequenceScreen.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

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
	
	if (param == "sq")
	{
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		displaySequenceNumberName();
	}
}

void DeleteSequenceScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 2:
        mpc.getLayeredScreen()->openScreen<DeleteAllSequencesScreen>();
		break;
	case 3:
        mpc.getLayeredScreen()->openScreen<SequenceScreen>();
		break;
	case 4:
		sequencer.lock()->move(0);
		sequencer.lock()->purgeSequence(sequencer.lock()->getActiveSequenceIndex());
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
		break;
	}
}

void DeleteSequenceScreen::displaySequenceNumberName()
{
	auto sequenceName = sequencer.lock()->getActiveSequence()->getName();
	findField("sq")->setText(StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2) + "-" + sequenceName);
}
