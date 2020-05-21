#include "EraseScreen.hpp"

#include <ui/Uis.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Label.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

using namespace moduru::lang;

using namespace std;

EraseScreen::EraseScreen(const int& layer)
	: ScreenComponent("erase", layer)
{
}

void EraseScreen::open()
{
	auto samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);

	auto seq = sequencer.lock()->getActiveSequence().lock();

	setTime0(0);
	setTime1(seq->getLastTick());
	
	displayErase();
	displayNotes();
	displayTime();
	displayTrack();
	displayType();
}

void EraseScreen::close()
{
	auto samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	samplerGui->deleteObserver(this);
}

void EraseScreen::turnWheel(int i)
{
	init();

	if (param.compare("track") == 0)
	{
		setTrack(track + i);
	}

	checkAllTimesAndNotes(i);

	if (param.compare("erase") == 0)
	{
		setErase(erase + i);
	}
	else if (param.compare("type") == 0)
	{
		setType(type + i);
	}
}

void EraseScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
	{
		auto startIndex = track - 1;
		auto lastIndex = track - 1;

		if (startIndex < 0)
		{
			startIndex = 0;
			lastIndex = 63;
		}

		auto midi = sequencer.lock()->getActiveTrack().lock()->getBusNumber() == 0;

		auto noteA = midi ? midiNote0 : samplerGui->getNote();
		auto noteB = midi ? midiNote1 : -1;

		auto seq = sequencer.lock()->getActiveSequence().lock();

		for (auto j = startIndex; j < lastIndex + 1; j++)
		{
			vector<int> removalIndices;
			auto t = seq->getTrack(j).lock();
			
			for (auto k = 0; k < t->getEvents().size(); k++)
			{
				auto e = t->getEvent(k).lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(e);

				if (e->getTick() >= time0 && e->getTick() <= time1)
				{
					string excludeClass;
					string includeClass;
					switch (erase) {
					case 0:
						if (ne) {
							auto nn = ne->getNote();
							if (midi && nn >= noteA && nn <= noteB) {
								removalIndices.push_back(k);
							}
							if (!midi && (noteA <= 34 || noteA == nn)) {
								removalIndices.push_back(k);
							}
						}
						else {
							removalIndices.push_back(k);
						}
						break;
					case 1:
						excludeClass = eventClassNames[type];
						if (string(typeid(e).name()).compare(excludeClass) != 0)
						{
							if (ne)
							{
								auto nn = ne->getNote();
							
								if (midi && nn >= noteA && nn <= noteB)
								{
									removalIndices.push_back(k);
								}
								
								if (!midi && (noteA > 34 && noteA != nn))
								{
									removalIndices.push_back(k);
								}
							}
							else
							{
								removalIndices.push_back(k);
							}
						}
						break;
					case 2:
						includeClass = eventClassNames[type];
						
						if (string(typeid(e).name()).compare(includeClass) == 0)
						{
							if (ne)
							{
								auto nn = ne->getNote();
							
								if (midi && nn >= noteA && nn <= noteB)
								{
									removalIndices.push_back(k);
								}
								
								if (!midi && (noteA <= 34 || noteA == nn))
								{
									removalIndices.push_back(k);
								}
							}
							else
							{
								removalIndices.push_back(k);
							}
						}
						break;
					}

				}
			}

			sort(begin(removalIndices), end(removalIndices));
			reverse(begin(removalIndices), end(removalIndices));

			for (int integer : removalIndices)
			{
				t->getEvents().erase(t->getEvents().begin() + integer);
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}

void EraseScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("padandnote") == 0)
	{
		displayNotes();
	}
}

void EraseScreen::displayTrack()
{
	string trackName = "";
	if (track == -1)
	{
		trackName = "ALL";
	}
	else
	{
		auto sequence = sequencer.lock()->getActiveSequence().lock();
		trackName = sequence->getTrack(track).lock()->getActualName();
	}
	findField("track").lock()->setTextPadded(track + 1, " ");
	findLabel("trackname").lock()->setText("-" + trackName);
}

void EraseScreen::displayTime()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock().get();
	findField("time0").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time0) + 1, "0");
	findField("time1").lock()->setTextPadded(SeqUtil::getBeat(sequence, time0) + 1, "0");
	findField("time2").lock()->setTextPadded(SeqUtil::getClock(sequence, time0), "0");
	findField("time3").lock()->setTextPadded(SeqUtil::getBarFromTick(sequence, time1) + 1, "0");
	findField("time4").lock()->setTextPadded(SeqUtil::getBeat(sequence, time1) + 1, "0");
	findField("time5").lock()->setTextPadded(SeqUtil::getClock(sequence, time1), "0");
}

void EraseScreen::displayErase()
{
	findField("erase").lock()->setText(eraseNames[erase]);
}

void EraseScreen::displayType()
{
	findField("type").lock()->Hide(erase == 0);
	if (erase > 0)
	{
		findField("type").lock()->setText(typeNames[type]);
	}
}

void EraseScreen::displayNotes()
{	
	init();

	if (erase != 0 && ((erase == 1 && type != 0) || (erase == 2 && type != 0)))
	{
		findField("notes0").lock()->Hide(true);
		findLabel("notes0").lock()->Hide(true);
		findField("notes1").lock()->Hide(true);
		findLabel("notes1").lock()->Hide(true);
		return;
	}

	auto bus = sequencer.lock()->getActiveTrack().lock()->getBusNumber();

	findField("notes0").lock()->Hide(false);
	findLabel("notes0").lock()->Hide(false);
	findField("notes1").lock()->Hide(bus != 0);
	findLabel("notes1").lock()->Hide(bus != 0);

	if (bus > 0)
	{
		findField("notes0").lock()->setSize(6 * 6 + 2, 8);

		if (samplerGui->getNote() != 34)
		{
			findField("notes0").lock()->setText(to_string(samplerGui->getNote()) + "/" + sampler.lock()->getPadName(samplerGui->getPad()));
		}
		else {
			findField("notes0").lock()->setText("ALL");
		}

	}
	else
	{
		findField("notes0").lock()->setSize(8 * 6, 8);
		findField("notes0").lock()->setText((StrUtil::padLeft(to_string(midiNote0), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote0]) + ")");
		findField("notes1").lock()->setText((StrUtil::padLeft(to_string(midiNote1), " ", 3) + "(" + mpc::ui::Uis::noteNames[midiNote1]) + ")");
	}
}

void EraseScreen::setTrack(int i)
{
	if (i < -1 || i > 63)
	{
		return;
	}

	track = i;
	displayTrack();
}

void EraseScreen::setErase(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	erase = i;
	displayErase();
}

void EraseScreen::setType(int i)
{
	if (i < 0 || i > 6)
	{
		return;
	}

	type = i;
	displayType();
}
