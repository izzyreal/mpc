#include "TempoChangeControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <cmath>

using namespace mpc::controls::sequencer::window;
using namespace mpc::sequencer;
using namespace std;

TempoChangeControls::TempoChangeControls()
	: AbstractSequencerControls()
{
}

void TempoChangeControls::left()
{
    init();
	if (param.length() == 2) {
        if(param[0] == 'a') {
            ls.lock()->setFocus("tempochange");
            return;
        }
    }
    super::left();
}

void TempoChangeControls::right()
{
	init();
	if (param.length() == 2) {
		if (param[0] == 'f') {
			ls.lock()->setFocus("initialtempo");
			return;
		}
	}
	super::right();
}

void TempoChangeControls::function(int j)
{
	super::function(j);
	auto yPos = -1;
	if (param.length() == 2)
		yPos = stoi(param.substr(1, 2));

	int nowDetected;
	std::shared_ptr<TempoChangeEvent> tce;
	auto seq = sequence.lock();
	
	auto tceList = seq->getTempoChangeEvents();
	switch (j) {
	case 1:
		if (yPos + swGui->getTempoChangeOffset() >= tceList.size()) return;
		if (tceList[swGui->getTempoChangeOffset() + yPos].lock()->getStepNumber() == 0) return;
		seq->removeTempoChangeEvent(swGui->getTempoChangeOffset() + yPos);
		seq->sortTempoChangeEvents();

		if (swGui->getTempoChangeOffset() + yPos == tceList.size()) swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() - 1);

		ls.lock()->openScreen("tempochange");
		ls.lock()->setFocus(string("a" + to_string(yPos)));
		break;
	case 2:
		nowDetected = -1;
		for (int i = 0; i < tceList.size(); i++) {
			if (tceList[i].lock()->getTick() == sequencer.lock()->getTickPosition()) {
				nowDetected = i;
				break;
			}
		}
		if (nowDetected == -1) {
			auto tce = seq->addTempoChangeEvent().lock();
			tce->setTick(sequencer.lock()->getTickPosition());
			seq->sortTempoChangeEvents();
		}
		else {
			if (nowDetected > swGui->getTempoChangeOffset() + 3 || nowDetected < swGui->getTempoChangeOffset()) {
				swGui->setTempoChangeOffset(nowDetected);
			}
			ls.lock()->setFocus(param.substr(0, 1) + to_string(nowDetected - swGui->getTempoChangeOffset()));
		}
		break;
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		tceList = seq->getTempoChangeEvents();
		if (tceList.size() == 1) {
			tce = seq->addTempoChangeEvent().lock();
			tce->setTick(seq->getLastTick());
			tce->setStepNumber(1);
		}
		else if (tceList.size() > 1) {
			if (param.length() != 2)
				return;
			auto lCurrent = current.lock();
			auto lNext = next.lock();
			auto lPrevious = previous.lock();
			if (yPos + swGui->getTempoChangeOffset() == 0) {
				if (lCurrent->getTick() == 1)
					return;
				tce = seq->addTempoChangeEvent().lock();
				tce->setTick(lNext->getTick() - 1);
			}
			else if (yPos + swGui->getTempoChangeOffset() > 0) {
				if (lCurrent->getTick() - 1 == lPrevious->getTick())
					return;
				tce = seq->addTempoChangeEvent().lock();
				tce->setTick(lCurrent->getTick() - 1);
			}
		}
		seq->sortTempoChangeEvents();
		ls.lock()->openScreen("tempochange");
		break;
	}
}

void TempoChangeControls::init()
{
	super::init();
	auto seq = sequence.lock();
	auto tceList = seq->getTempoChangeEvents();
	yPos = -1;
	if (param.length() == 2) {
		yPos = stoi(param.substr(1, 2));
	}

	if (param.length() == 2) {
		int nextPosition = yPos + swGui->getTempoChangeOffset() + 1;
		if (tceList.size() > nextPosition) {
			next = tceList[nextPosition];
		}
		int currentPosition = yPos + swGui->getTempoChangeOffset();

		if (currentPosition + 1 > tceList.size()) return;

		current = tceList[currentPosition];
		auto previousPosition = yPos + swGui->getTempoChangeOffset() - 1;
		if (previousPosition >= 0) {
			previous = tceList[previousPosition];
		}
	}
}

void TempoChangeControls::turnWheel(int j)
{
	init();
	auto seq = sequence.lock();
	auto tceList = seq->getTempoChangeEvents();
	if (param.compare("tempochange") == 0) {
		seq->setTempoChangeOn(j > 0);
	}
	else if (param.compare("initialtempo") == 0) {
		auto tce = tceList[0].lock();
		seq->setInitialTempo(BCMath(seq->getInitialTempo().toDouble() + (j / 10.0)));
	}

	for (int i = 0; i < 3; i++) {
		auto ts = seq->getTimeSignature();
		auto tce = swGui->getVisibleTempoChanges()[i].lock();

		if (param.compare("b" + to_string(i)) == 0) {
			if (j > 0) {
				tce->plusOneBar(ts.getNumerator(), ts.getDenominator(), next.lock().get());
			}
			else {
				tce->minusOneBar(ts.getNumerator(), ts.getDenominator(), previous.lock().get());
			}
		}
		else if (param.compare("c" + to_string(i)) == 0) {
			if (j > 0) {
				tce->plusOneBeat(ts.getDenominator(), next.lock().get());
			}
			else {
				tce->minusOneBeat(ts.getDenominator(), previous.lock().get());
			}
		}
		else if (param.compare("d" + to_string(i)) == 0) {
			if (j > 0) {
				tce->plusOneClock(next.lock().get());
			}
			else {
				tce->minusOneClock(previous.lock().get());
			}
		}
		else if (param.compare("e" + to_string(i)) == 0) {
			tce->setRatio(tce->getRatio() + j);
		}
	}
}

void TempoChangeControls::down()
{
	init();
	auto tce1 = swGui->getVisibleTempoChanges()[1].lock();
	auto tce2 = swGui->getVisibleTempoChanges()[2].lock();

	if (param.compare("tempochange") == 0) {
		ls.lock()->setFocus("e0");
	}
	else if (param.compare("initialtempo") == 0) {
		ls.lock()->setFocus("f0");
	}
	else if (param.length() == 2) {
		if ((yPos == 1 && !tce1) || (yPos == 2 && !tce2)) {
			return;
		}
		else if (yPos == 1 && !tce2) {
			ls.lock()->setFocus("a2");
			return;
		}
		else if (yPos == 2) {
			swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() + 1);
			if (swGui->getTempoChangeOffset() + yPos == sequence.lock()->getTempoChangeEvents().size() && param[0] != 'a') {
				ls.lock()->setFocus("a2");
			}
			return;
		}
		ls.lock()->setFocus(string(param.substr(0, 1) + to_string(yPos + 1)));
	}
}

void TempoChangeControls::up()
{
	init();
	if (param.length() == 2) {
		if (yPos == 0) {
			if (swGui->getTempoChangeOffset() == 0) {
				if (param.compare("e0") == 0) {
					ls.lock()->setFocus("tempochange");
				}
				else if (param.compare("f0") == 0) {
					ls.lock()->setFocus("initialtempo");
				}
				return;
			}
			swGui->setTempoChangeOffset(swGui->getTempoChangeOffset() - 1);
			return;
		}
		ls.lock()->setFocus(param.substr(0, 1) + to_string(yPos - 1));
		return;
	}
}
