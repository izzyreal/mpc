#include "SequenceStateView.hpp"
#include "sequencer/Track.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/EventRef.hpp"

#include <algorithm>

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

void Track::correctTimeRange(const int startPos, const int endPos,
                             const int stepLength, const int swingPercentage,
                             const int lowestNote, const int highestNote,
                             const bool shiftLater, const int shiftAmount) const
{
    const auto seq = parent->getSnapshot(parent->getSequenceIndex());

    // Capture the original selection and ticks before any queued updates can
    // move notes into or out of the range.
    const auto events = getEventRange(startPos, endPos);
    for (const auto &event : events)
    {
        const auto &original = event->snapshot;
        if (original.type == EventType::NoteOn &&
            original.noteNumber >= lowestNote &&
            original.noteNumber <= highestNote)
        {
            updateEventTick(event->handle,
                            timingCorrectAndShiftTick(
                                seq, original.tick, stepLength, swingPercentage,
                                shiftLater, shiftAmount));
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

mpc::Tick Track::timingCorrectAndShiftTick(const SequenceStateView &seq,
                                           Tick tick, const int stepLength,
                                           const int swingPercentage,
                                           const bool shiftLater,
                                           const int shiftAmount)
{
    if (stepLength > 1)
    {
        tick = timingCorrectTick(seq, 0, seq.getLastBarIndex(), tick,
                                 stepLength, swingPercentage);
    }

    tick += shiftLater ? shiftAmount : -shiftAmount;
    return std::clamp<Tick>(tick, 0, seq.getLastTick());
}
