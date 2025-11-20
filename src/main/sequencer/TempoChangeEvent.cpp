#include "TempoChangeEvent.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SeqUtil.hpp"

using namespace mpc::sequencer;

TempoChangeEvent::TempoChangeEvent(const std::function<performance::Event()> &getSnapshot, Sequence *parent)
    : TempoChangeEvent(getSnapshot, parent, 1000)
{
}

TempoChangeEvent::TempoChangeEvent(const std::function<performance::Event()> &getSnapshot, Sequence *parent, const int ratio)
    : Event(getSnapshot)
{
    this->ratio = ratio;
    this->parent = parent;
}

TempoChangeEvent::TempoChangeEvent(const TempoChangeEvent &event) : Event(event)
{
    setRatio(event.getRatio());
}

void TempoChangeEvent::setParent(Sequence *newParent)
{
    parent = newParent;
}

void TempoChangeEvent::plusOneBar(const TempoChangeEvent *next)
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
}

void TempoChangeEvent::minusOneBar(const TempoChangeEvent *previous)
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
}

void TempoChangeEvent::plusOneBeat(const TempoChangeEvent *next)
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
}

void TempoChangeEvent::minusOneBeat(const TempoChangeEvent *previous)
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
}

void TempoChangeEvent::plusOneClock(const TempoChangeEvent *next)
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
}

void TempoChangeEvent::minusOneClock(const TempoChangeEvent *previous)
{
    if (previous != nullptr)
    {
        if (tick == previous->getTick() + 1)
        {
            return;
        }
    }

    tick--;
}

void TempoChangeEvent::setRatio(const int i)
{
    ratio = std::clamp(i, 100, 9998);
}

int TempoChangeEvent::getRatio() const
{
    return ratio;
}

int TempoChangeEvent::getBar(const int n, const int d) const
{
    const auto barLength = static_cast<int>(96 * (4.0 / d) * n);
    const auto bar = tick / barLength;
    return bar;
}

int TempoChangeEvent::getBeat(const int n, const int d) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / d));
    const auto beat = tick / beatLength % n;
    return beat;
}

int TempoChangeEvent::getClock(const int denominator) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / denominator));
    const auto clock = tick % beatLength;
    return clock;
}

double TempoChangeEvent::getTempo() const
{
    const auto tempo = parent->getInitialTempo() * ratio * 0.001;

    if (tempo < 30.0)
    {
        return 30.0;
    }
    if (tempo > 300.0)
    {
        return 300.0;
    }

    return tempo;
}
