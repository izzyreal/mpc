#include <lcdgui/screens/window/EraseScreen.hpp>

#include <lcdgui/screens/window/SeqWindowUtil.hpp>

#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/window/EraseGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace std;

EraseScreen::EraseScreen(const int& layer)
	: ScreenComponent("erase", layer)
{
	eventClassNames = vector<string>{ "com.mpc.sequencer.NoteEvent", "com.mpc.sequencer.PitchBendEvent", "com.mpc.sequencer.ControlChangeEvent", "com.mpc.sequencer.ProgramChangeEvent", "com.mpc.sequencer.ChannelPressureEvent", "com.mpc.sequencer.PolyPressureEvent", "com.mpc.sequencer.SystemExclusiveEvent" };
}

void EraseScreen::turnWheel(int i)
{
	init();

	auto egui = Mpc::instance().getUis().lock()->getEraseGui();

	if (param.compare("track") == 0) {
		egui->setTrack(egui->getTrack() + i);
	}

	SeqWindowUtil::checkAllTimesAndNotes(i);

	if (param.compare("erase") == 0) {
		egui->setErase(egui->getErase() + i);
	}
	else if (param.compare("type") == 0) {
		egui->setType(egui->getType() + i);
	}
}

void EraseScreen::function(int i)
{
	init();
	auto egui = Mpc::instance().getUis().lock()->getEraseGui();

	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
	{
		shared_ptr<Sequence> seq;
		int startIndex;
		int lastIndex;
		int erase;
		int type;
		bool midi;
		int notea;
		int noteb;
		seq = sequencer.lock()->getActiveSequence().lock();
		startIndex = egui->getTrack() - 1;
		lastIndex = egui->getTrack() - 1;

		if (startIndex < 0)
		{
			startIndex = 0;
			lastIndex = 63;
		}

		erase = egui->getErase();
		type = egui->getType();
		midi = track.lock()->getBusNumber() == 0;

		auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

		notea = midi ? swGui->getMidiNote0() : samplerGui->getNote();
		noteb = midi ? swGui->getMidiNote1() : -1;

		for (auto j = startIndex; j < lastIndex + 1; j++) {
			vector<int> removalIndices;
			auto t = seq->getTrack(j).lock();
			for (auto k = 0; k < t->getEvents().size(); k++) {
				auto e = t->getEvent(k).lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(e);

				if (e->getTick() >= swGui->getTime0() && e->getTick() <= swGui->getTime1())
				{
					string excludeClass;
					string includeClass;
					switch (erase) {
					case 0:
						if (ne) {
							auto nn = ne->getNote();
							if (midi && nn >= notea && nn <= noteb) {
								removalIndices.push_back(k);
							}
							if (!midi && (notea <= 34 || notea == nn)) {
								removalIndices.push_back(k);
							}
						}
						else {
							removalIndices.push_back(k);
						}
						break;
					case 1:
						excludeClass = eventClassNames[type];
						if (string(typeid(e).name()).compare(excludeClass) != 0) {
							if (ne) {
								auto nn = ne->getNote();
								if (midi && nn >= notea && nn <= noteb) {
									removalIndices.push_back(k);
								}
								if (!midi && (notea > 34 && notea != nn)) {
									removalIndices.push_back(k);
								}
							}
							else {
								removalIndices.push_back(k);
							}
						}
						break;
					case 2:
						includeClass = eventClassNames[type];
						if (string(typeid(e).name()).compare(includeClass) == 0) {
							if (ne) {
								auto nn = ne->getNote();
								if (midi && nn >= notea && nn <= noteb) {
									removalIndices.push_back(k);
								}
								if (!midi && (notea <= 34 || notea == nn)) {
									removalIndices.push_back(k);
								}
							}
							else {
								removalIndices.push_back(k);
							}
						}
						break;
					}

				}
			}

			sort(begin(removalIndices), end(removalIndices));
			reverse(begin(removalIndices), end(removalIndices));

			for (int integer : removalIndices) {
				t->getEvents().erase(t->getEvents().begin() + integer);
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}
