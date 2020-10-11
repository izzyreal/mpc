#include "TrMoveScreen.hpp"

#include "EventsScreen.hpp"

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

TrMoveScreen::TrMoveScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "tr-move", layerIndex)
{
}

void TrMoveScreen::open()
{
	findBackground().lock()->SetDirty();
	findLabel("selecttrack").lock()->setText("Select track");
	findLabel("tomove").lock()->setText("to move.");

	displaySq();
	displayTrFields();
	displayTrLabels();
}

void TrMoveScreen::turnWheel(int i)
{
	init();

	if (param.find("tr") != string::npos && i > 0)
	{
		goUp();
	}
	else if (param.find("tr") != string::npos && i < 0)
	{
		goDown();
	}
	else if (param.compare("sq") == 0)
	{
		auto eventsScreen = dynamic_pointer_cast<EventsScreen>(mpc.screens->getScreenComponent("events"));
		eventsScreen->setFromSq(sequencer.lock()->getActiveSequenceIndex() + i);
		displaySq();
		displayTrFields();
		displayTrLabels();
	}
}

void TrMoveScreen::up()
{
	init();
	if (param.compare("tr") == 0)
	{
		goUp();
	}
}

void TrMoveScreen::down()
{
	init();
	if (param.compare("tr") == 0)
	{
		goDown();
	}
	else
	{
		ScreenComponent::down();
		ls.lock()->setFunctionKeysArrangement(1);
	}
}

void TrMoveScreen::left()
{
	init();

	if (param.compare("sq") == 0)
	{
		return;
	}

	if (isSelected())
	{
		return;
	}

	ScreenComponent::left();
	ls.lock()->setFunctionKeysArrangement(0);
}

void TrMoveScreen::right()
{
	init();

	if (param.compare("tr") == 0)
	{
		return;
	}

	ScreenComponent::right();
	ls.lock()->setFunctionKeysArrangement(1);
}

void TrMoveScreen::function(int i)
{
	init();
	switch (i)
	{
		// Intentional fall-through
	case 0:
	case 1:
	case 3:
	{
		auto eventsScreen = dynamic_pointer_cast<EventsScreen>(mpc.screens->getScreenComponent("events"));
		eventsScreen->tab = i;
		openScreen(eventsScreen->tabNames[eventsScreen->tab]);
		break;
	}
	case 4:
		if (isSelected())
		{
			cancel();
		}
		break;
	case 5:
		if (param.compare("sq") == 0)
		{
			break;
		}

		if (isSelected())
		{
			auto sequence = mpc.getSequencer().lock()->getActiveSequence().lock();
			insert(sequence.get());
		}
		else
		{
			select();
		}
		break;
	}
}

void TrMoveScreen::displayTrLabels()
{
	string tr0 = "";
	string tr1 = "";
	string tr0Name = "";
	string tr1Name = "";

	auto tr0Index = currentTrackIndex - 1;
	auto tr1Index = 0;
	
	if (isSelected() && tr0Index >= selectedTrackIndex)
	{
		tr0Index++;
	}

	tr1Index = tr0Index + 2;

	if (isSelected())
	{
		tr1Index--;

		if (tr0Index == selectedTrackIndex)
		{
			tr0Index--;
		}
		
		if (tr1Index == selectedTrackIndex)
		{
			tr1Index++;
		}
	}

	auto eventsScreen = dynamic_pointer_cast<EventsScreen>(mpc.screens->getScreenComponent("events"));
	auto sequence = mpc.getSequencer().lock()->getActiveSequence().lock();

	if (tr0Index >= 0)
	{
		tr0Name = sequence->getTrack(tr0Index).lock()->getName();
		tr0 += "Tr:" + StrUtil::padLeft(to_string(tr0Index + 1), "0", 2) + "-" + tr0Name;
	}
	else
	{
		tr0 = "";
	}
	if (tr1Index < 64)
	{
		tr1Name = sequence->getTrack(tr1Index).lock()->getName();
		tr1 += "Tr:" + StrUtil::padLeft(to_string(tr1Index + 1), "0", 2) + "-" + tr1Name;
	}
	else
	{
		tr1 = "";
	}
	
	if (tr0.compare("") == 0)
	{
		findLabel("tr0").lock()->Hide(true);
	}
	else
	{
		findLabel("tr0").lock()->Hide(false);
		findLabel("tr0").lock()->setText(tr0);
	}

	if (tr1.compare("") == 0)
	{
		findLabel("tr1").lock()->Hide(true);
	}
	else
	{
		findLabel("tr1").lock()->Hide(false);
		findLabel("tr1").lock()->setText(tr1);
	}
}

void TrMoveScreen::displayTrFields()
{
	auto eventsScreen = dynamic_pointer_cast<EventsScreen>(mpc.screens->getScreenComponent("events"));
	auto sequence = mpc.getSequencer().lock()->getActiveSequence().lock();
	
	if (isSelected())
	{
		findLabel("selecttrack").lock()->Hide(true);
		findLabel("tomove").lock()->Hide(true);
		findField("tr").lock()->setLocation(9, 26);

		auto tr0Name = sequence->getTrack(selectedTrackIndex).lock()->getName();
		
		if (tr0Name.length() < 10)
		{
			tr0Name = StrUtil::padRight(tr0Name, " ", 9) + u8"\u00CD";
		}
		
		findField("tr").lock()->setText("Tr:" + StrUtil::padLeft(to_string(selectedTrackIndex + 1), "0", 2) + "-" + tr0Name);
	}
	else
	{
		findLabel("selecttrack").lock()->Hide(false);
		findLabel("tomove").lock()->Hide(false);
		findField("tr").lock()->setLocation(108, 26);

		findField("tr").lock()->setText("Tr:" + StrUtil::padLeft(to_string(currentTrackIndex + 1), "0", 2) + "-" + sequence->getTrack(currentTrackIndex).lock()->getName());
	}
}

void TrMoveScreen::displaySq()
{
	auto sequence = mpc.getSequencer().lock()->getActiveSequence().lock();
	findField("sq").lock()->setText(StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
	auto sequenceName = "-" + sequence->getName();
	findLabel("sq-name").lock()->setText(sequenceName);
}

bool TrMoveScreen::isSelected()
{
	if (selectedTrackIndex != -1)
	{
		return true;
	}
	return false;
}

void TrMoveScreen::goUp()
{
	if (currentTrackIndex == 0)
	{
		return;
	}
	
	currentTrackIndex--;
	
	displayTrLabels();
	displayTrFields();
}

void TrMoveScreen::goDown()
{
	if (currentTrackIndex == 63)
	{
		return;
	}
	currentTrackIndex++;
	displayTrLabels();
	displayTrFields();
}

void TrMoveScreen::select()
{
	selectedTrackIndex = currentTrackIndex;
	displayTrLabels();
	displayTrFields();
	ls.lock()->setFunctionKeysArrangement(2);
	SetDirty();
}

void TrMoveScreen::cancel()
{
	selectedTrackIndex = -1;
	displayTrLabels();
	displayTrFields();
	ls.lock()->setFunctionKeysArrangement(1);
	SetDirty();
}

void TrMoveScreen::insert(mpc::sequencer::Sequence* s)
{
	s->moveTrack(selectedTrackIndex, currentTrackIndex);
	selectedTrackIndex = -1;
	displayTrLabels();
	displayTrFields();
	ls.lock()->setFunctionKeysArrangement(1);
	SetDirty();
}
