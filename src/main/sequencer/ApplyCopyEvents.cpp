#include "sequencer/SequencerStateManager.hpp"

#include "lcdgui/screens/UserScreen.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequencer.hpp"
#include "StrUtil.hpp"

#include <vector>

using namespace mpc::sequencer;

namespace
{
void removeEventWithoutLock(SequencerStateManager *manager, TrackState &track,
                            EventData *e)
{
    const EventData *head = track.eventsHead;

    int removedIndex = 0;
    {
        const EventData *it = head;
        int idx = 0;
        while (it && it != e)
        {
            it = it->next;
            idx++;
        }
        removedIndex = idx;
    }

    if (e->prev)
    {
        e->prev->next = e->next;
    }

    if (e->next)
    {
        e->next->prev = e->prev;
    }

    if (track.eventsHead == e)
    {
        track.eventsHead = e->next;
    }

    if (track.playEventIndex > removedIndex)
    {
        --track.playEventIndex;
    }
    else if (track.playEventIndex == removedIndex)
    {
        track.playEventIndex =
            std::max(mpc::EventIndex(0), track.playEventIndex);
    }

    e->prev = nullptr;
    e->next = nullptr;

    manager->returnEventToPool(e);
}

void ensureTrackUsedForCopy(Sequencer *sequencer, TrackState &track,
                            mpc::TrackIndex trackIndex)
{
    if (track.used || trackIndex == mpc::TempoChangeTrackIndex)
    {
        track.used = true;
        return;
    }

    track.name = sequencer->getDefaultTrackName(trackIndex);
    track.used = true;
}

void initializeUnusedDestinationSequence(SequencerStateManager *manager,
                                         Sequencer *sequencer,
                                         SequenceState &destSeq,
                                         mpc::SequenceIndex destSequenceIndex,
                                         int sourceLastBarIndex)
{
    const auto userScreen =
        sequencer->getScreens()->get<mpc::lcdgui::ScreenId::UserScreen>();

    destSeq.initializeDefaults();
    destSeq.initialTempo = userScreen->getTempo();
    destSeq.loopEnabled = userScreen->isLoopEnabled();
    destSeq.lastBarIndex = mpc::BarIndex(sourceLastBarIndex);
    destSeq.firstLoopBarIndex = mpc::BarIndex(0);
    destSeq.lastLoopBarIndex = mpc::EndOfSequence;
    destSeq.used = true;
    destSeq.tempoChangeEnabled = true;
    destSeq.name = sequencer->getDefaultSequenceName() +
                   mpc::StrUtil::padLeft(
                       std::to_string(destSequenceIndex + 1), "0", 2);

    const auto timeSig = userScreen->getTimeSig();

    for (int i = 0; i <= sourceLastBarIndex; ++i)
    {
        destSeq.timeSignatures[i] = timeSig;
        destSeq.barLengths[i] = timeSig.getBarLength();
    }

    for (auto &track : destSeq.tracks)
    {
        track.initializeDefaults();
        track.deviceIndex = userScreen->getDevice();
        track.velocityRatio = userScreen->getVelo();
        track.busType = userScreen->getBusType();
        track.programChange = userScreen->getPgm();
    }

    EventData *tempoEvent = manager->acquireEvent();
    if (tempoEvent)
    {
        tempoEvent->type = EventType::TempoChange;
        tempoEvent->tick = 0;
        tempoEvent->amount = 1000;
        tempoEvent->sequenceIndex = destSequenceIndex;
        tempoEvent->trackIndex = mpc::TempoChangeTrackIndex;
        manager->insertAcquiredEvent(destSeq.tracks[mpc::TempoChangeTrackIndex],
                                     tempoEvent);
    }
}
} // namespace

void SequencerStateManager::applyCopyEvents(const CopyEvents &m) noexcept
{
    auto &lock = trackLocks[m.destSequenceIndex][m.destTrackIndex];

    if (!lock.try_acquire())
    {
        enqueue(m);
        return;
    }

    const auto segLength = m.sourceEndTick - m.sourceStartTick;
    const auto destOffset = m.destStartTick - m.sourceStartTick;

    int destNumerator = -1;
    int destDenominator = -1;
    int destBarLength = -1;

    const SequenceIndex sourceSequenceIndexToUse =
        m.sourceSequenceIndex >= MinSequenceIndex
            ? m.sourceSequenceIndex
            : activeState.selectedSequenceIndex;

    const auto &sourceSeq = activeState.sequences[sourceSequenceIndexToUse];
    auto &destSeq = activeState.sequences[m.destSequenceIndex];
    auto &destTrack = destSeq.tracks[m.destTrackIndex];

    std::vector<EventData> sourceEvents;
    const EventData *src = sourceSeq.tracks[m.sourceTrackIndex].eventsHead;
    while (src)
    {
        if (src->tick >= m.sourceEndTick)
        {
            break;
        }

        if (src->tick >= m.sourceStartTick &&
            (src->type != EventType::NoteOn ||
             m.sourceNoteRange.contains(src->noteNumber)))
        {
            sourceEvents.push_back(*src);
        }

        src = src->next;
    }

    if (!destSeq.used)
    {
        initializeUnusedDestinationSequence(this, sequencer, destSeq,
                                            m.destSequenceIndex,
                                            sourceSeq.lastBarIndex);
    }

    const SequenceStateView destSeqView(destSeq);
    const auto destSequenceBarCount = destSeq.lastBarIndex + 1;

    for (int i = 0; i < destSequenceBarCount; i++)
    {
        const auto firstTickOfBar = destSeqView.getFirstTickOfBar(BarIndex(i));
        const auto barLength = destSeqView.getBarLength(i);

        if (m.destStartTick >= firstTickOfBar &&
            m.destStartTick <= firstTickOfBar + barLength)
        {
            const auto ts = destSeqView.getTimeSignature(i);
            destNumerator = ts.numerator;
            destDenominator = ts.denominator;
            destBarLength = barLength;
            break;
        }
    }

    if (destBarLength < 0 && destSequenceBarCount > 0)
    {
        const auto ts = destSeqView.getTimeSignature(destSequenceBarCount - 1);
        destNumerator = ts.numerator;
        destDenominator = ts.denominator;
        destBarLength = destSeqView.getBarLength(destSequenceBarCount - 1);
    }

    if (destBarLength <= 0)
    {
        lock.release();
        return;
    }

    const auto minimumRequiredNewSequenceLength = m.destStartTick + segLength;
    const auto ticksToAdd =
        minimumRequiredNewSequenceLength - destSeqView.getLastTick();

    const auto barsToAdd = std::max(
        0, static_cast<int>(ceil(static_cast<float>(ticksToAdd) / destBarLength)));

    const auto initialLastBarIndex = destSeqView.getLastBarIndex();

    for (int i = 0; i < barsToAdd; i++)
    {
        const auto afterBar = initialLastBarIndex + i + 1;
        if (afterBar >= 998)
        {
            break;
        }

        applyMessage(InsertBars{m.destSequenceIndex, 1, afterBar});
        const TimeSignature ts{TimeSigNumerator(destNumerator),
                               TimeSigDenominator(destDenominator)};
        applyMessage(SetTimeSignature{m.destSequenceIndex, afterBar, ts});
    }

    if (!m.copyModeMerge)
    {
        EventData *prev = nullptr;
        EventData *cur = destTrack.eventsHead;

        while (cur)
        {
            const bool inRange =
                cur->tick >= destOffset &&
                cur->tick < destOffset + segLength * m.copyCount;

            EventData *next = cur->next;

            if (inRange)
            {
                removeEventWithoutLock(this, destTrack, cur);
            }
            else
            {
                prev = cur;
            }

            cur = next;
        }
    }

    for (const auto &sourceEvent : sourceEvents)
    {
        for (int copyIndex = 0; copyIndex < m.copyCount; copyIndex++)
        {
            const int tickCandidate =
                sourceEvent.tick + destOffset + copyIndex * segLength;

            if (tickCandidate >= destSeqView.getLastTick())
            {
                break;
            }

            EventData *ev = acquireEvent();
            if (!ev)
            {
                continue;
            }
            *ev = sourceEvent;

            ev->sequenceIndex = m.destSequenceIndex;
            ev->trackIndex = m.destTrackIndex;
            ev->tick = tickCandidate;

            ensureTrackUsedForCopy(sequencer, destTrack, m.destTrackIndex);
            insertAcquiredEvent(destTrack, ev);
        }
    }

    lock.release();

    applyMessage(SetSelectedSequenceIndex{m.destSequenceIndex});
}
