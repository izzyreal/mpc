#include "LoadASequenceScreen.hpp"

#include <disk/AbstractDisk.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

LoadASequenceScreen::LoadASequenceScreen(const int layerIndex) 
	: ScreenComponent("load-a-sequence", layerIndex)
{
}

void LoadASequenceScreen::open()
{
	displayLoadInto();
	displayFile();
}

void LoadASequenceScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("load-into") == 0)
	{
		setLoadInto(loadInto + i);
	}
}

void LoadASequenceScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("load");
		sequencer.lock()->clearPlaceHolder();
		break;
	case 4:
		auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(Screens::getScreenComponent("load-a-sequence"));
		sequencer.lock()->movePlaceHolderTo(loadInto);
		sequencer.lock()->setActiveSequenceIndex(loadInto);
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void LoadASequenceScreen::setLoadInto(int i)
{
	if (i < 0 || i > 98)
	{
		return;
	}
	loadInto = i;
	displayLoadInto();
}

void LoadASequenceScreen::displayLoadInto()
{
	auto loadASequenceScreen = dynamic_pointer_cast<LoadASequenceScreen>(Screens::getScreenComponent("load-a-sequence"));
	findField("load-into").lock()->setTextPadded(loadASequenceScreen->loadInto + 1, "0");
	findLabel("load-into0").lock()->setText("-" + Mpc::instance().getSequencer().lock()->getSequence(loadASequenceScreen->loadInto).lock()->getName());
}

void LoadASequenceScreen::displayFile()
{
	auto s = sequencer.lock()->getPlaceHolder().lock();
	findLabel("file").lock()->setText("File:" + StrUtil::toUpper(mpc::disk::AbstractDisk::padFileName16(s->getName())) + ".MID");
	return;
}
