#include <controls/sequencer/EditSequenceControls.hpp>

#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace mpc::sequencer;
using namespace std;

EditSequenceControls::EditSequenceControls()
	: AbstractSequencerControls()
{
}

void EditSequenceControls::function(int i)
{
	init();
	auto lFromSeq = fromSeq.lock();
	auto lToSeq = toSeq.lock();
	int sourceStart, sourceEnd, destStart, segLength;
	shared_ptr<Track> sourceTrack;
	auto uis = Mpc::instance().getUis().lock();
	switch (i) {
	case 1:
		uis->getBarCopyGui()->setFromSq(editSequenceGui->getFromSq());
		ls.lock()->openScreen("barcopy");
		break;
	case 2:
		uis->getTrMoveGui()->setSq(editSequenceGui->getFromSq());
		ls.lock()->openScreen("trmove");
		break;
	case 3:
		ls.lock()->openScreen("user");
		break;
	case 5:
		sourceStart = editSequenceGui->getTime0();
		sourceEnd = editSequenceGui->getTime1();
		segLength = sourceEnd - sourceStart;
		sourceTrack = lFromSeq->getTrack(editSequenceGui->getTr0()).lock();

		if (editSequenceGui->getEditFunctionNumber() == 0) {
			destStart = editSequenceGui->getStartTicks();
			auto destOffset = destStart - sourceStart;
			if (!lToSeq->isUsed()) {
				lToSeq->init(lFromSeq->getLastBar());
			}

			auto destTrack = lToSeq->getTrack(editSequenceGui->getTr1()).lock();

			if (!editSequenceGui->isModeMerge()) {
				for (auto& e : destTrack->getEvents()) {
					auto tick = e.lock()->getTick();
					if (tick >= destOffset && tick < destOffset + (segLength * editSequenceGui->getCopies())) {
						destTrack->removeEvent(e);
					}
				}
			}

			for (auto& e : sourceTrack->getEvents()) {
				auto event = e.lock();
				auto ne = dynamic_pointer_cast<NoteEvent>(event);
				if (ne) {
					if (sourceTrack->getBusNumber() == 0) {
						auto mn0 = editSequenceGui->getMidiNote0();
						auto mn1 = editSequenceGui->getMidiNote1();
						if (ne->getNote() < mn0 || ne->getNote() > mn1) continue;

					}
					else {
						auto dn = editSequenceGui->getDrumNote();
						if (dn != 34 && dn != ne->getNote()) continue;
					}
				}

				if (event->getTick() >= sourceEnd) {
					break;
				}

				if (event->getTick() >= sourceStart) {
					for (int copy = 0; copy < editSequenceGui->getCopies(); copy++) {
						int tickCandidate = event->getTick() + destOffset + (copy * segLength);
						if (tickCandidate < lToSeq->getLastTick()) {
							auto temp = destTrack->cloneEvent(event).lock();
							temp->setTick(tickCandidate);
						}
					}
				}
			}
			
			destTrack->sortEvents();
		}
		else if (editSequenceGui->getEditFunctionNumber() == 1) {
			for (auto& e : sourceTrack->getEvents()) {
				auto event = e.lock();
				auto n = dynamic_pointer_cast<NoteEvent>(event);
				if (n) {
					if (editSequenceGui->getDurationMode() == 0) {
						n->setDuration(n->getDuration() + editSequenceGui->getDurationValue());
					}
					if (editSequenceGui->getDurationMode() == 1) {
						n->setDuration(n->getDuration() - editSequenceGui->getDurationValue());
					}
					if (editSequenceGui->getDurationMode() == 2) {
						n->setDuration((n->getDuration() * (editSequenceGui->getDurationValue() / 100.0)));
					}
					if (editSequenceGui->getDurationMode() == 3) {
						n->setDuration(editSequenceGui->getDurationValue());
					}
				}
			}
		}
		else if (editSequenceGui->getEditFunctionNumber() == 2) {
			for (auto& e : sourceTrack->getEvents()) {
				auto event = e.lock();
				auto n = dynamic_pointer_cast<NoteEvent>(event);
				if (n) {
					if (editSequenceGui->getVelocityMode() == 0) {
						n->setVelocity(n->getVelocity() + editSequenceGui->getVelocityValue());
					}
					else if (editSequenceGui->getVelocityMode() == 1) {
						n->setVelocity(n->getVelocity() - editSequenceGui->getVelocityValue());
					}
					else if (editSequenceGui->getVelocityMode() == 2) {
						n->setVelocity((n->getVelocity() * (editSequenceGui->getVelocityValue() / 100.0)));
					}
					else if (editSequenceGui->getVelocityMode() == 3) {
						n->setVelocity(editSequenceGui->getVelocityValue());
					}
				}
			}
		}
		else if (editSequenceGui->getEditFunctionNumber() == 3) {
			for (auto& e : sourceTrack->getEvents()) {
				auto event = e.lock();
				auto n = dynamic_pointer_cast<NoteEvent>(event);
				if (n) {
					n->setNote(n->getNote() + editSequenceGui->getVelocityValue());
					break;
				}
			}
		}
		ls.lock()->openScreen("sequencer");
	}
}

void EditSequenceControls::turnWheel(int i)
{
	init();
	auto lFromSeq = fromSeq.lock();
	auto lToSeq = toSeq.lock();
	
	if (param.compare("time0") == 0) {
		editSequenceGui->setTime0(editSequenceGui->setBarNumber(editSequenceGui->getBarNumber(lFromSeq.get(), editSequenceGui->getTime0()) + i, lFromSeq.get(), editSequenceGui->getTime0()));
	}
	else if (param.compare("time1") == 0) {
		editSequenceGui->setTime0(editSequenceGui->setBeatNumber(editSequenceGui->getBeatNumber(lFromSeq.get(), editSequenceGui->getTime0()) + i, lFromSeq.get(), editSequenceGui->getTime0()));
	}
	else if (param.compare("time2") == 0) {
		editSequenceGui->setTime0(editSequenceGui->setClockNumber(editSequenceGui->getClockNumber(lFromSeq.get(), editSequenceGui->getTime0()) + i, lFromSeq.get(), editSequenceGui->getTime0()));
	}
	else if (param.compare("time3") == 0) {
		editSequenceGui->setTime1(editSequenceGui->setBarNumber(editSequenceGui->getBarNumber(lFromSeq.get(), editSequenceGui->getTime1()) + i, lFromSeq.get(), editSequenceGui->getTime1()));
	}
	else if (param.compare("time4") == 0) {
		editSequenceGui->setTime1(editSequenceGui->setBeatNumber(editSequenceGui->getBeatNumber(lFromSeq.get(), editSequenceGui->getTime1()) + i, lFromSeq.get(), editSequenceGui->getTime1()));
	}
	else if (param.compare("time5") == 0) {
		editSequenceGui->setTime1(editSequenceGui->setClockNumber(editSequenceGui->getClockNumber(lFromSeq.get(), editSequenceGui->getTime1()) + i, lFromSeq.get(), editSequenceGui->getTime1()));
	}
	else if (param.compare("start0") == 0) {
		editSequenceGui->setStartTicks(editSequenceGui->setBarNumber(editSequenceGui->getBarNumber(lToSeq.get(), editSequenceGui->getStartTicks()) + i, lToSeq.get(), editSequenceGui->getStartTicks()));
	}
	else if (param.compare("start1") == 0) {
		editSequenceGui->setStartTicks(editSequenceGui->setBeatNumber(editSequenceGui->getBeatNumber(lToSeq.get(), editSequenceGui->getStartTicks()) + i, lToSeq.get(), editSequenceGui->getStartTicks()));
	}
	else if (param.compare("start2") == 0) {
		editSequenceGui->setStartTicks(editSequenceGui->setClockNumber(editSequenceGui->getClockNumber(lToSeq.get(), editSequenceGui->getStartTicks()) + i, lToSeq.get(), editSequenceGui->getStartTicks()));
	}
	else if (param.compare("editfunction") == 0) {
		editSequenceGui->setEditFunctionNumber(editSequenceGui->getEditFunctionNumber() + i);
	}
	else if (param.compare("drumnote") == 0) {
		editSequenceGui->setDrumNote(editSequenceGui->getDrumNote() + i);
	}
	else if (param.compare("midinote0") == 0) {
		editSequenceGui->setMidiNote0(editSequenceGui->getMidiNote0() + i);
	}
	else if (param.compare("midinote1") == 0) {
		editSequenceGui->setMidiNote1(editSequenceGui->getMidiNote1() + i);
	}
	else if (param.compare("fromsq") == 0) {
		editSequenceGui->setFromSq(editSequenceGui->getFromSq() + i);
		fromSeq = sequencer.lock()->getSequence(editSequenceGui->getFromSq());
		lFromSeq = fromSeq.lock();
		if (editSequenceGui->getTime1() > lFromSeq->getLastTick()) {
			editSequenceGui->setTime1(lFromSeq->getLastTick());
		}
	}
	else if (param.compare("tr0") == 0) {
		editSequenceGui->setTr0(editSequenceGui->getTr0() + i);
	}
	else if (param.compare("tosq") == 0) {
		editSequenceGui->setToSq(editSequenceGui->getToSq() + i);
		toSeq = sequencer.lock()->getSequence(editSequenceGui->getToSq());
		lToSeq = toSeq.lock();
		if (editSequenceGui->getStartTicks() > lToSeq->getLastTick()) {
			editSequenceGui->setStartTicks(lToSeq->getLastTick());
		}
	}
	else if (param.compare("tr1") == 0) {
		editSequenceGui->setTr1(editSequenceGui->getTr1() + i);
	}
	else if (param.compare("mode") == 0) {
		if (editSequenceGui->getEditFunctionNumber() == 0) {
			editSequenceGui->setModeMerge(i > 0);
		}
		else if (editSequenceGui->getEditFunctionNumber() == 1) {
			editSequenceGui->setDurationMode(editSequenceGui->getDurationMode() + i);
		}
		else if (editSequenceGui->getEditFunctionNumber() == 2) {
			editSequenceGui->setVelocityMode(editSequenceGui->getVelocityMode() + i);
		}
		else if (editSequenceGui->getEditFunctionNumber() == 3) {
			editSequenceGui->setTransposeAmount(editSequenceGui->getTransposeAmount() + i);
		}
	}
	else if (param.compare("copies") == 0) {
		if (editSequenceGui->getEditFunctionNumber() == 0) {
			editSequenceGui->setCopies(editSequenceGui->getCopies() + i);
		}
		else if (editSequenceGui->getEditFunctionNumber() == 1) {
			editSequenceGui->setDurationValue(editSequenceGui->getDurationValue() + i);
		}
		else if (editSequenceGui->getEditFunctionNumber() == 2) {
			editSequenceGui->setVelocityValue(editSequenceGui->getVelocityValue() + i);
		}
	}
}
