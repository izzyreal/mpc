#include "TempoChangeEvent.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SeqUtil.hpp"

using namespace mpc::sequencer;

TempoChangeEvent::TempoChangeEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch, Sequence *parent)
    : EventRef(ptr, snapshot, dispatch), parent(parent)
{
}

void TempoChangeEvent::setParent(Sequence *newParent)
{
    parent = newParent;
}

void TempoChangeEvent::plusOneBar(const TempoChangeEvent *next) const
{
    auto candidate =
        parent->getFirstTickOfBar(SeqUtil::getBar(parent, handle->tick) + 1);

    if (candidate > parent->getLastTick())
    {
        candidate = parent->getLastTick();
    }

    if (next != nullptr)
    {
        if (candidate >= next->getTick())
        {
            candidate = next->getTick() - 1;
        }
    }

    setTick(candidate);
}

void TempoChangeEvent::minusOneBar(const TempoChangeEvent *previous) const
{
    auto candidate =
        parent->getFirstTickOfBar(SeqUtil::getBar(parent, handle->tick) - 1);

    if (candidate < 0)
    {
        candidate = 0;
    }

    if (previous != nullptr)
    {
        if (candidate <= previous->getTick())
        {
            candidate = previous->getTick() + 1;
        }
    }

    setTick(candidate);
}

void TempoChangeEvent::plusOneBeat(const TempoChangeEvent *next) const
{
    const auto oldTick = handle->tick;

    auto candidate =
        parent->getFirstTickOfBeat(SeqUtil::getBar(parent, oldTick),
                                   SeqUtil::getBeat(parent, oldTick) + 1);

    if (candidate >= parent->getLastTick())
    {
        candidate = parent->getLastTick() - 1;
    }

    if (next != nullptr)
    {
        if (candidate >= next->getTick())
        {
            candidate = next->getTick() - 1;
        }
    }

    setTick(candidate);
}

void TempoChangeEvent::minusOneBeat(const TempoChangeEvent *previous) const
{
    const auto oldTick = handle->tick;
    auto candidate =
        parent->getFirstTickOfBeat(SeqUtil::getBar(parent, oldTick),
                                   SeqUtil::getBeat(parent, oldTick) - 1);

    if (candidate < 0)
    {
        candidate = 0;
    }

    if (previous != nullptr)
    {
        if (candidate <= previous->getTick())
        {
            candidate = previous->getTick() + 1;
        }
    }

    setTick(candidate);
}

void TempoChangeEvent::plusOneClock(const TempoChangeEvent *next) const
{
    const auto oldTick = handle->tick;

    if (next != nullptr && oldTick == next->getTick() - 1)
    {
        return;
    }

    if (oldTick + 1 >= parent->getLastTick())
    {
        return;
    }

    auto candidate = oldTick + 1;

    if (candidate > parent->getLastTick())
    {
        candidate = parent->getLastTick();
    }

    setTick(candidate);
}

void TempoChangeEvent::minusOneClock(const TempoChangeEvent *previous) const
{
    const auto oldTick = handle->tick;

    if (previous != nullptr)
    {
        if (oldTick == previous->getTick() + 1)
        {
            return;
        }
    }

    setTick(oldTick - 1);
}

void TempoChangeEvent::setRatio(const int i) const
{
    auto e = snapshot;
    e.amount = std::clamp(i, 100, 9998);
    dispatch(UpdateEvent{handle, e});
}

int TempoChangeEvent::getRatio() const
{
    return snapshot.amount;
}

int TempoChangeEvent::getBar(const int n, const int d) const
{
    const auto barLength = static_cast<int>(96 * (4.0 / d) * n);
    const auto bar = handle->tick / barLength;
    return bar;
}

int TempoChangeEvent::getBeat(const int n, const int d) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / d));
    const auto beat = handle->tick / beatLength % n;
    return beat;
}

int TempoChangeEvent::getClock(const int denominator) const
{
    const auto beatLength = static_cast<int>(96 * (4.0 / denominator));
    const auto clock = handle->tick % beatLength;
    return clock;
}

double TempoChangeEvent::getTempo() const
{
    const auto tempo = parent->getInitialTempo() * getRatio() * 0.001;

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
