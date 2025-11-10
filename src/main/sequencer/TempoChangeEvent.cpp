#include "TempoChangeEvent.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SeqUtil.hpp"

using namespace mpc::sequencer;

TempoChangeEvent::TempoChangeEvent(Sequence *parent)
    : TempoChangeEvent(parent, 1000)
{
}

TempoChangeEvent::TempoChangeEvent(Sequence *parent, int ratio)
{
    this->ratio = ratio;
    this->parent = parent;
}

mpc::sequencer::TempoChangeEvent::TempoChangeEvent(
    const TempoChangeEvent &event)
    : Event(event)
{
    setRatio(event.getRatio());
}

void TempoChangeEvent::setParent(Sequence *newParent)
{
    parent = newParent;
}

void TempoChangeEvent::plusOneBar(TempoChangeEvent *next)
{
    tick = parent->getFirstTickOfBar(SeqUtil::getBar(parent, tick) + 1);

    if (tick > parent->getLastTick())
    {
        tick = parent->getLastTick();
    }

    if (next != nullptr)
    {
        if (tick >= next->getTick())
        {
            tick = next->getTick() - 1;
        }
    }

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::minusOneBar(TempoChangeEvent *previous)
{
    tick = parent->getFirstTickOfBar(SeqUtil::getBar(parent, tick) - 1);

    if (tick < 0)
    {
        tick = 0;
    }

    if (previous != nullptr)
    {
        if (tick <= previous->getTick())
        {
            tick = previous->getTick() + 1;
        }
    }

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::plusOneBeat(TempoChangeEvent *next)
{
    tick = parent->getFirstTickOfBeat(SeqUtil::getBar(parent, tick),
                                      SeqUtil::getBeat(parent, tick) + 1);

    if (tick >= parent->getLastTick())
    {
        tick = parent->getLastTick() - 1;
    }

    if (next != nullptr)
    {
        if (tick >= next->getTick())
        {
            tick = next->getTick() - 1;
        }
    }

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::minusOneBeat(TempoChangeEvent *previous)
{
    tick = parent->getFirstTickOfBeat(SeqUtil::getBar(parent, tick),
                                      SeqUtil::getBeat(parent, tick) - 1);

    if (tick < 0)
    {
        tick = 0;
    }

    if (previous != nullptr)
    {
        if (tick <= previous->getTick())
        {
            tick = previous->getTick() + 1;
        }
    }

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::plusOneClock(TempoChangeEvent *next)
{
    if (next != nullptr && tick == next->getTick() - 1)
    {
        return;
    }

    if (tick + 1 >= parent->getLastTick())
    {
        return;
    }

    tick++;

    if (tick > parent->getLastTick())
    {
        tick = parent->getLastTick();
    }

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::minusOneClock(TempoChangeEvent *previous)
{
    if (previous != nullptr)
    {
        if (tick == previous->getTick() + 1)
        {
            return;
        }
    }

    tick--;

    notifyObservers(std::string("tempo-change"));
}

void TempoChangeEvent::setRatio(int i)
{
    if (i < 100 || i > 9998)
    {
        return;
    }

    ratio = i;

    notifyObservers(std::string("tempo-change"));
}

int TempoChangeEvent::getRatio() const
{
    return ratio;
}

int TempoChangeEvent::getBar(int n, int d) const
{
    const auto barLength = (int)(96 * (4.0 / d) * n);
    const auto bar = (int)(tick / barLength);
    return bar;
}

int TempoChangeEvent::getBeat(int n, int d) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / d));
    const auto beat = (static_cast<int>(tick / beatLength)) % n;
    return beat;
}

int TempoChangeEvent::getClock(int denominator) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / denominator));
    const auto clock = static_cast<int>(tick % beatLength);
    return clock;
}

double TempoChangeEvent::getTempo() const
{
    const auto tempo = parent->getInitialTempo() * ratio * 0.001;

    if (tempo < 30.0)
    {
        return 30.0;
    }

    else if (tempo > 300.0)
    {
        return 300.0;
    }

    return tempo;
}
