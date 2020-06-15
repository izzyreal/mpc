#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::sequencer;
using namespace std;

TempoChangeEvent::TempoChangeEvent(Sequence* parent)
	: TempoChangeEvent(parent, 1000)
{
}

TempoChangeEvent::TempoChangeEvent(Sequence* parent, int ratio)
{
	this->ratio = ratio;
	this->parent = parent;
}

TempoChangeEvent::TempoChangeEvent(Sequence* parent, int ratio, int step)
{
	this->ratio = ratio;
	this->parent = parent;
	this->stepNumber = step;
}

void TempoChangeEvent::setParent(Sequence* newParent) {
	parent = newParent;
}

void TempoChangeEvent::plusOneBar(int numerator, int denominator, TempoChangeEvent* next)
{
    if (stepNumber == 0) return;

    tick = tick + (numerator * (96 * (4.0 / denominator)));
    if (tick > parent->getLastTick())
        tick = parent->getLastTick();

    if (next != nullptr) {
        if(tick >= next->getTick()) {
            tick = next->getTick() - 1;
        }
    }
    
    notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::minusOneBar(int numerator, int denominator, TempoChangeEvent* previous)
{
	if (stepNumber == 0) return;

	tick = tick - (numerator * (96 * (4.0 / denominator)));
	if (tick < 0) tick = 0;

	if (previous != nullptr) {
		if (tick <= previous->getTick()) {
			tick = previous->getTick() + 1;
		}
	}
	
	notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::plusOneBeat(int denominator, TempoChangeEvent* next)
{
	if (stepNumber == 0) return;
	tick = tick + 96 * (4.0 / denominator);
	if (tick > parent->getLastTick()) tick = parent->getLastTick();

	if (next != nullptr) {
		if (tick >= next->getTick()) {
			tick = next->getTick() - 1;
		}
	}
	
	notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::minusOneBeat(int denominator, TempoChangeEvent* previous)
{
	if (stepNumber == 0) return;

	tick = tick - 96 * (4.0 / denominator);
	if (tick < 0) tick = 0;

	if (previous != nullptr) {
		if (tick <= previous->getTick()) {
			tick = previous->getTick() + 1;
		}
	}
	
	notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::plusOneClock(TempoChangeEvent* next)
{
	if (stepNumber == 0) return;
	if (next != nullptr) {
		if (tick == next->getTick() - 1) return;
	}
	tick++;
	if (tick > parent->getLastTick())
		tick = parent->getLastTick();

	
	notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::minusOneClock(TempoChangeEvent* previous)
{
	if (stepNumber == 0 || tick == 0) return;

	if (previous != nullptr) {
		if (tick == previous->getTick() + 1) return;
	}
	tick--;

	
	notifyObservers(string("tempo-change"));
}

void TempoChangeEvent::setRatio(int i)
{
	if (i < 100 || i > 9998) return;
	ratio = i;
	
	notifyObservers(string("tempo-change"));
}

int TempoChangeEvent::getRatio()
{
    return ratio;
}

void TempoChangeEvent::setStepNumber(int i)
{
	if (i < 0) return;
	stepNumber = i;
	
	notifyObservers(string("tempo-change"));
}

int TempoChangeEvent::getStepNumber()
{
    return stepNumber;
}

int TempoChangeEvent::getBar(int n, int d)
{
	auto barLength = (int)(96 * (4.0 / d) * n);
	auto bar = (int)(tick / barLength);
	return bar;
}

int TempoChangeEvent::getBeat(int n, int d)
{
	auto beatLength = static_cast<int>(96 * (4.0 / d));
	auto beat = (static_cast<int>(tick / beatLength)) % n;
	return beat;
}

int TempoChangeEvent::getClock(int n, int d)
{
	auto beatLength = static_cast<int>(96 * (4.0 / d));
	auto clock = static_cast<int>(tick % beatLength);
	return clock;
}

BCMath TempoChangeEvent::getTempo()
{
	auto tempoDouble = parent->getInitialTempo().toDouble() * (ratio / 1000.0);
	auto str = to_string(tempoDouble);
	auto length = (int)(str.find(".")) + 2;
	auto tempo = BCMath(str.substr(0, length));
	if (tempo.toDouble() < 30.0) tempo = BCMath("30.0");
	if (tempo.toDouble() > 300.0) tempo = BCMath("300.0");
	return tempo;
}

void TempoChangeEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<TempoChangeEvent>(dest.lock());
	lDest->setRatio(getRatio());
	lDest->setStepNumber(getStepNumber());
}
