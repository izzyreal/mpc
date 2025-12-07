#include "SequenceStateView.hpp"
#include "sequencer/Track.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/TrackStateView.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

void Track::correctTimeRange(const int startPos, const int endPos,
                             const int stepLength, const int swingPercentage,
                             const int lowestNote, const int highestNote) const
{
    const auto seq = getActiveSequence();
    int accumBarLengths = 0;
    auto fromBar = 0;
    auto toBar = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq->getBarLength(i);

        if (accumBarLengths >= startPos)
        {
            fromBar = i;
            break;
        }
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq->getBarLength(i);

        if (accumBarLengths > endPos)
        {
            toBar = i;
            break;
        }
    }

    for (const auto &event : getSnapshot(getIndex())->getNoteEvents())
    {
        if (event->tick >= endPos)
        {
            break;
        }

        if (event->tick >= startPos && event->tick < endPos &&
            event->noteNumber >= lowestNote && event->noteNumber <= highestNote)
        {
            timingCorrect(*parent->getSnapshot(parent->getSequenceIndex()), fromBar, toBar, event,
                          event->tick, stepLength, swingPercentage);
        }
    }

    removeDoubles();
}

void Track::timingCorrect(const SequenceStateView &seq, const int fromBar,
                          const int toBar, EventData *e, const Tick eventTick,
                          const int stepLength, const int swingPercentage) const
{
    updateEventTick(e, timingCorrectTick(seq, fromBar, toBar, eventTick,
                                         stepLength, swingPercentage));
}

int Track::timingCorrectTick(const SequenceStateView &seq, const int fromBar,
                             const int toBar, int tick, const int stepLength,
                             const int swingPercentage)
{
    int accumBarLengths = 0;
    int previousAccumBarLengths = 0;
    auto barNumber = 0;
    auto numberOfSteps = 0;
    int segmentStart = 0;
    int segmentEnd = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        if (i < fromBar)
        {
            segmentStart += seq.getBarLength(i);
        }

        if (i <= toBar)
        {
            segmentEnd += seq.getBarLength(i);
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        accumBarLengths += seq.getBarLength(i);

        if (tick < accumBarLengths && tick >= previousAccumBarLengths)
        {
            barNumber = i;
            break;
        }

        previousAccumBarLengths = accumBarLengths;
    }

    for (int i = 1; i < 1000; i++)
    {
        if (seq.getBarLength(barNumber) - i * stepLength < 0)
        {
            numberOfSteps = i - 1;
            break;
        }
    }

    int currentBarStart = 0;

    for (int i = 0; i < barNumber; i++)
    {
        currentBarStart += seq.getBarLength(i);
    }

    for (int i = 0; i <= numberOfSteps; i++)
    {
        const int stepStart = (i - 1) * stepLength + stepLength / 2;

        if (const int stepEnd = i * stepLength + stepLength / 2;
            tick - currentBarStart >= stepStart &&
            tick - currentBarStart <= stepEnd)
        {
            tick = i * stepLength + currentBarStart;

            if (tick >= segmentEnd)
            {
                tick = segmentStart;
            }

            break;
        }
    }

    if ((stepLength == 24 || stepLength == 48) &&
        (tick + stepLength) % (stepLength * 2) == 0 && swingPercentage > 50)
    {
        const int swingOffset =
            (swingPercentage - 50.f) / 25.f * (stepLength / 2.f);
        tick += swingOffset;
    }

    return tick;
}

void Track::shiftTiming(EventData *e, const Tick eventTick, const bool later,
                        const int amount, const int lastTick) const
{
    int amountToUse = amount;

    if (!later)
    {
        amountToUse *= -1;
    }

    int newEventTick = eventTick + amountToUse;

    if (newEventTick < 0)
    {
        newEventTick = 0;
    }
    else if (newEventTick > lastTick)
    {
        newEventTick = lastTick;
    }

    updateEventTick(e, newEventTick);
}
