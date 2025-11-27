#include "sequencer/NonRtSequencerStateManager.hpp"

#include "NonRtSequenceStateView.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "SequencerMessage.hpp"
#include "SequencerStateManager.hpp"
#include "Transport.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "sequencer/NonRtSequencerStateWorker.hpp"

using namespace mpc::sequencer;

NonRtSequencerStateManager::NonRtSequencerStateManager(
    const std::function<SampleRate()> &getSampleRate, Sequencer *sequencer,
    NonRtSequencerStateWorker *worker)
    : AtomicStateExchange([](NonRtSequencerState &) {}),
      getSampleRate(getSampleRate), worker(worker), sequencer(sequencer)
{
}

NonRtSequencerStateManager::~NonRtSequencerStateManager()
{
    //    printf("~NonRtSequencerStateManager\n");
}
void NonRtSequencerStateManager::applyMessage(
    const NonRtSequencerMessage &msg) noexcept
{
    myApplyMessage(msg);
}

template <typename V, typename... Ts>
bool isVariantAnyOf(const V &v, std::variant<Ts...> const &)
{
    return std::visit(
        [](auto const &x)
        {
            using X = std::decay_t<decltype(x)>;
            return ((std::is_same_v<X, Ts>) || ...);
        },
        v);
}

void NonRtSequencerStateManager::myApplyMessage(
    const NonRtSequencerMessage &msg,
    const bool autoRefreshPlaybackState) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T,
                                         RefreshPlaybackStateWhileNotPlaying>)
            {
                worker->refreshPlaybackState(
                    activeState.transport.positionQuarterNotes, 0,
                    m.onComplete);
            }
            else if constexpr (std::is_same_v<T,
                                              RefreshPlaybackStateWhilePlaying>)
            {
                worker->refreshPlaybackState(
                    activeState.transport.positionQuarterNotes,
                    CurrentTimeInSamples, m.onComplete);
            }
            else if constexpr (std::is_same_v<T, UpdatePlaybackState>)
            {
                // printf("Applying UpdatePlaybackState\n");
                activeState.playbackState = m.playbackState;
                actions.push_back(m.onComplete);
            }
            else if constexpr (std::is_same_v<T, UpdateTrackIndexOfAllEvents>)
            {
                // printf("Applying UpdateTrackIndexOfAllEvents\n");
                for (auto &e : activeState.sequences[m.sequence]
                                   .tracks[m.trackIndex]
                                   .events)
                {
                    e.trackIndex = m.trackIndex;
                }
            }
            else if constexpr (std::is_same_v<T, FinalizeNonLiveNoteEvent>)
            {
                // printf("Applying FinalizeNonLiveNoteEvent\n");
                for (auto &e :
                     activeState.sequences[m.noteOnEvent.sequenceIndex]
                         .tracks[m.noteOnEvent.trackIndex]
                         .events)
                {
                    if (e.beingRecorded && e.duration == NoDuration &&
                        e.noteNumber == m.noteOnEvent.noteNumber)
                    {
                        e.duration = m.duration;
                        e.beingRecorded = false;
                        break;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, SetInitialTempo>)
            {
                activeState.sequences[m.sequenceIndex].initialTempo = m.initialTempo;
            }
            else if constexpr (std::is_same_v<T, UpdateEvent>)
            {
                // printf("Applying UpdateEvent\n");
                for (auto &e : activeState.sequences[m.payload.sequenceIndex]
                                   .tracks[m.payload.trackIndex]
                                   .events)
                {
                    if (e.eventId == m.payload.eventId)
                    {
                        e = m.payload;
                    }
                }
            }
            else if constexpr (std::is_same_v<T, InsertEvent>)
            {
                // printf("Applying InsertEvent\n");
                assert(m.eventState.eventId != NoEventId);
                auto &events = activeState.sequences[m.eventState.sequenceIndex]
                                   .tracks[m.eventState.trackIndex]
                                   .events;

                if (m.eventState.type == EventType::NoteOn &&
                    !m.allowMultipleNoteEventsWithSameNoteOnSameTick)
                {
                    for (auto it = events.begin(); it != events.end(); ++it)
                    {
                        if (it->type == EventType::NoteOn)
                        {
                            if (it->tick == m.eventState.tick &&
                                it->noteNumber == m.eventState.noteNumber)
                            {
                                events.erase(it);
                                break;
                            }
                        }
                    }
                }

                const bool insertRequired =
                    !events.empty() && events.back().tick >= m.eventState.tick;

                if (insertRequired)
                {
                    const auto insertAt = std::find_if(
                        events.begin(), events.end(),
                        [tick = m.eventState.tick](const EventState &e)
                        {
                            return e.tick > tick;
                        });

                    if (insertAt == events.end())
                    {
                        events.emplace_back(m.eventState);
                    }
                    else
                    {
                        events.emplace(insertAt, m.eventState);
                    }
                }
                else
                {
                    events.emplace_back(m.eventState);
                }

                // printf("Inserted event in sequence %i track %i tick %llu\n",
                //     m.eventState.sequenceIndex.get(),
                //     m.eventState.trackIndex.get(), m.eventState.tick);

                actions.push_back(m.onComplete);
            }
            else if constexpr (std::is_same_v<T, ClearEvents>)
            {
                // printf("Applying ClearEvents\n");
                activeState.sequences[m.sequence]
                    .tracks[m.track]
                    .events.clear();
            }
            else if constexpr (std::is_same_v<T, RemoveDoubles>)
            {
                // printf("Applying RemoveDoubles\n");
                auto eventCounter = 0;
                std::vector<int> deleteIndexList;
                std::vector<int> notesAtTick;
                int lastTick = -100;

                auto &events =
                    activeState.sequences[m.sequence].tracks[m.track].events;

                for (auto &e : events)
                {
                    if (e.type == EventType::NoteOn)
                    {
                        if (lastTick != e.tick)
                        {
                            notesAtTick.clear();
                        }

                        bool contains = false;

                        for (const auto &n : notesAtTick)
                        {
                            if (n == e.noteNumber)
                            {
                                contains = true;
                                break;
                            }
                        }

                        if (!contains)
                        {
                            notesAtTick.push_back(e.noteNumber);
                        }
                        else
                        {
                            deleteIndexList.push_back(eventCounter);
                        }

                        lastTick = e.tick;
                    }
                    eventCounter++;
                }

                std::reverse(deleteIndexList.begin(), deleteIndexList.end());

                for (const auto &i : deleteIndexList)
                {
                    events.erase(events.begin() + i);
                }
            }
            else if constexpr (std::is_same_v<T, UpdateEventTick>)
            {
                // printf("Applying UpdateEventTick\n");
                auto &events = activeState.sequences[m.eventState.sequenceIndex]
                                   .tracks[m.eventState.trackIndex]
                                   .events;
                auto it = std::find_if(
                    events.begin(), events.end(),
                    [eventId = m.eventState.eventId](const EventState &e)
                    {
                        return e.eventId == eventId;
                    });
                assert(it != events.end());
                const auto oldIndex = it - events.begin();
                const Tick newTick = m.newTick;

                EventState ev = events[oldIndex];

                events.erase(events.begin() + oldIndex);

                auto it2 =
                    std::lower_bound(events.begin(), events.end(), newTick,
                                     [](const EventState &e, const Tick t)
                                     {
                                         return e.tick < t;
                                     });

                events.insert(it2, ev);
                const auto newIndex = it2 - events.begin();
                events[newIndex].tick = newTick;
            }
            else if constexpr (std::is_same_v<T, RemoveEvent>)
            {
                // printf("Applying RemoveEvent\n");
                auto &events =
                    activeState.sequences[m.sequence].tracks[m.track].events;
                auto it =
                    std::find_if(events.begin(), events.end(),
                                 [eventId = m.eventId](const EventState &e)
                                 {
                                     return e.eventId == eventId;
                                 });
                assert(it != events.end());
                events.erase(it);
            }
            else if constexpr (std::is_same_v<T, SetPositionQuarterNotes>)
            {
                // printf("Applying SetPositionQuarterNotes\n");
                activeState.transport.positionQuarterNotes =
                    m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T,
                                              SetPlayStartPositionQuarterNotes>)
            {
                // printf("Applying SetPlayStartPositionQuarterNotes\n");
                activeState.transport.playStartPositionQuarterNotes =
                    m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                // printf("Applying BumpPositionByTicks\n");
                const double delta = Sequencer::ticksToQuarterNotes(m.ticks);
                activeState.transport.positionQuarterNotes += delta;
            }
            else if constexpr (std::is_same_v<T, SwitchToNextSequence>)
            {
                // printf("Applying SwitchToNextSequence\n");
                constexpr bool setPositionTo0 = false;
                enqueue(
                    SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
            }
            else if constexpr (std::is_same_v<T, SetSelectedSequenceIndex>)
            {
                printf("Applying SetSelectedSequenceIndex with index %i\n", m.sequenceIndex.get());
                activeState.selectedSequenceIndex = m.sequenceIndex;

                if (m.setPositionTo0)
                {
                    activeState.transport.positionQuarterNotes = 0;
                }
            }
            else if constexpr (std::is_same_v<T, Stop>)
            {
                // printf("Applying Stop\n");
                applyStopMessage();
            }
            else if constexpr (std::is_same_v<T, Play>)
            {
                // printf("Applying Play\n");
                applyPlayMessage();
            }
            else if constexpr (std::is_same_v<T, Record>)
            {
                activeState.transport.recording = true;
                applyMessage(Play{});
            }
            else if constexpr (std::is_same_v<T, RecordFromStart>)
            {
                activeState.transport.recording = true;
                applyMessage(PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, Overdub>)
            {
                activeState.transport.overdubbing = true;
                applyMessage(Play{});
            }
            else if constexpr (std::is_same_v<T, OverdubFromStart>)
            {
                activeState.transport.overdubbing = true;
                applyMessage(PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, UpdateRecording>)
            {
                activeState.transport.recording = m.recording;
            }
            else if constexpr (std::is_same_v<T, UpdateOverdubbing>)
            {
                activeState.transport.overdubbing = m.overdubbing;
            }
            else if constexpr (std::is_same_v<T, SwitchRecordToOverdub>)
            {
                activeState.transport.recording = false;
                activeState.transport.overdubbing = true;
                applyMessage(Play{});
            }
            else if constexpr (std::is_same_v<T, PlayFromStart>)
            {
                if (activeState.transport.positionQuarterNotes == 0)
                {
                    applyMessage(Play{});
                }
                else
                {
                    activeState.transport.positionQuarterNotes = 0;
                    auto onComplete = [this]
                    {
                        enqueue(Play{});
                    };
                    enqueue(RefreshPlaybackStateWhileNotPlaying{onComplete});
                }
            }
            else if constexpr (std::is_same_v<T, UpdateCountEnabled>)
            {
                activeState.transport.countEnabled = m.enabled;
            }
            else if constexpr (std::is_same_v<T, UpdateBarLength>)
            {
                // printf("Applying UpdateBarLength\n");
                activeState.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                    m.length;
            }
            else if constexpr (std::is_same_v<T, UpdateBarLengths>)
            {
                // printf("Applying UpdateBarLengths\n");
                activeState.sequences[m.sequenceIndex].barLengths =
                    m.barLengths;
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignatures>)
            {
                // printf("Applying UpdateTimeSignatures\n");
                auto &seq = activeState.sequences[m.sequenceIndex];

                seq.timeSignatures = m.timeSignatures;

                for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
                {
                    seq.barLengths[i] = seq.timeSignatures[i].getBarLength();
                }
            }
            else if constexpr (std::is_same_v<T, CopyEvents>)
            {
                applyCopyEvents(m);
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignature>)
            {
                // printf("Applying UpdateTimeSignature\n");
                activeState.sequences[m.sequenceIndex]
                    .timeSignatures[m.barIndex] = m.timeSignature;
                activeState.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                    m.timeSignature.getBarLength();
            }
            else if constexpr (std::is_same_v<T, SetLastBarIndex>)
            {
                activeState.sequences[m.sequenceIndex].lastBarIndex =
                    m.barIndex;
            }
            else if constexpr (std::is_same_v<T, InsertBars>)
            {
                applyInsertBars(m);
            }
            else if constexpr (std::is_same_v<T, UpdateEvents>)
            {
                // printf("Applying UpdateEvents\n");
                activeState.sequences[m.sequence].tracks[m.track].events =
                    m.eventStates;

                EventId eventId = MinEventId;
                for (auto &e :
                     activeState.sequences[m.sequence].tracks[m.track].events)
                {
                    e.sequenceIndex = m.sequence;
                    e.trackIndex = m.track;
                    e.eventId = eventId++;
                }
            }
            else if constexpr (std::is_same_v<T, UpdateSequenceEvents>)
            {
                printf("Applying UpdateSequenceEvents\n");

                auto &sequence = activeState.sequences[m.sequenceIndex];

                for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; ++i)
                {
                    sequence.tracks[i].events.clear();
                }

                EventId eventId = MinEventId;

                for (const auto &e : m.eventStates)
                {
                    auto &events = sequence.tracks[e.trackIndex].events;
                    events.push_back(e);
                    events.back().sequenceIndex = m.sequenceIndex;
                    events.back().eventId = eventId++;
                }
            }
        },
        msg);

    if (!autoRefreshPlaybackState)
    {
        return;
    }

    const auto isPlaying = sequencer->getTransport()->isPlaying();
    const auto playbackState =
        sequencer->getNonRtStateManager()->getSnapshot().getPlaybackState();

    if (std::holds_alternative<SetPositionQuarterNotes>(msg))
    {
        if (isPlaying)
        {
            return;
        }

        publishState();

        if (activeState.transport.positionQuarterNotes >
                playbackState.safeValidUntilQuarterNote ||
            activeState.transport.positionQuarterNotes <
                playbackState.safeValidFromQuarterNote)
        {
            applyMessage(RefreshPlaybackStateWhileNotPlaying{});
        }

        return;
    }

    if (isPlaying &&
        isVariantAnyOf(msg, MessagesThatInvalidPlaybackStateWhilePlaying{}))
    {
        publishState();
        applyMessage(RefreshPlaybackStateWhilePlaying{});
    }
    else if (!isPlaying &&
             isVariantAnyOf(msg,
                            MessagesThatInvalidPlaybackStateWhileNotPlaying{}))
    {
        publishState();
        applyMessage(RefreshPlaybackStateWhileNotPlaying{});
    }
}

void NonRtSequencerStateManager::applyPlayMessage() noexcept
{
    if (activeState.transport.sequencerRunning)
    {
        return;
    }

    const auto transport = sequencer->getTransport();

    transport->setEndOfSong(false);

    const auto songScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::SongScreen>();

    const auto currentSong =
        sequencer->getSong(songScreen->getSelectedSongIndex());

    const bool songMode = sequencer->isSongModeEnabled();

    // if (songMode)
    // {
    //     if (!currentSong->isUsed())
    //     {
    //         return;
    //     }
    //
    //     if (fromStart)
    //     {
    //         const int oldSongSequenceIndex =
    //         sequencer->getSongSequenceIndex(); songScreen->setOffset(-1); if
    //         (sequencer->getSongSequenceIndex() != oldSongSequenceIndex)
    //         {
    //             sequencer->setSelectedSequenceIndex(
    //                 sequencer->getSongSequenceIndex(), true);
    //         }
    //     }
    //
    //     if (songScreen->getOffset() + 1 > currentSong->getStepCount() - 1)
    //     {
    //         return;
    //     }
    //
    //     int step = songScreen->getOffset() + 1;
    //
    //     if (step > currentSong->getStepCount())
    //     {
    //         step = currentSong->getStepCount() - 1;
    //     }
    //
    //     if (const std::shared_ptr<Step> currentStep =
    //             currentSong->getStep(step).lock();
    //         !sequencer->getSequence(currentStep->getSequence())->isUsed())
    //     {
    //         return;
    //     }
    // }

    const auto countMetronomeScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>();

    const auto countInMode = countMetronomeScreen->getCountInMode();

    // std::optional<int64_t> positionQuarterNotesToStartPlayingFrom =
    //     std::nullopt;

    // if (!transport->isCountEnabled() || countInMode == 0 ||
    //     (countInMode == 1 && !transport->isRecordingOrOverdubbing()))
    // {
    //     if (fromStart && activeState.transportState.positionQuarterNotes !=
    //     0)
    //     {
    //         positionQuarterNotesToStartPlayingFrom = 0;
    //     }
    // }
    //
    const auto activeSequence = sequencer->getSelectedSequence();

    // if (transport->isCountEnabled() && !songMode)
    // {
    //     if (countInMode == 2 ||
    //         (countInMode == 1 && transport->isRecordingOrOverdubbing()))
    //     {
    //         if (fromStart)
    //         {
    //             positionQuarterNotesToStartPlayingFrom =
    //                 Sequencer::ticksToQuarterNotes(
    //                     activeSequence->getLoopStart());
    //         }
    //         else
    //         {
    //             positionQuarterNotesToStartPlayingFrom =
    //                 Sequencer::ticksToQuarterNotes(
    //                     activeSequence->getFirstTickOfBar(
    //                         transport->getCurrentBarIndex()));
    //         }
    //
    //         transport->setCountInStartPosTicks(Sequencer::quarterNotesToTicks(
    //             activeState.transportState.positionQuarterNotes));
    //
    //         transport->setCountInEndPosTicks(activeSequence->getLastTickOfBar(
    //             transport->getCurrentBarIndex()));
    //         transport->setCountingIn(true);
    //     }
    // }

    if (!songMode)
    {
        if (!activeSequence->isUsed())
        {
            transport->setRecording(false);
            transport->setOverdubbing(false);
            return;
        }

        activeSequence->initLoop();

        if (transport->isRecordingOrOverdubbing())
        {
            sequencer->storeSelectedSequenceInUndoPlaceHolder();
        }
    }

    if (sequencer->isBouncePrepared())
    {
        sequencer->startBouncing();
    }
    else
    {
        // if (positionQuarterNotesToStartPlayingFrom)
        // {
        //     if (*positionQuarterNotesToStartPlayingFrom !=
        //     activeState.transportState.positionQuarterNotes)
        //     {
        //         activeState.transportState.positionQuarterNotes =
        //         *positionQuarterNotesToStartPlayingFrom;
        //
        //     }
        // }

        activeState.transport.sequencerRunning = true;
    }
}

void NonRtSequencerStateManager::applyStopMessage() noexcept
{
    activeState.transport.sequencerRunning = false;
    // const bool bouncing = sequencer.isBouncing();
    //
    // if (!isPlaying() && !bouncing)
    // {
    //     if (const auto snapshot =
    //     sequencer.getNonRtStateManager()->getSnapshot();
    //         snapshot.getPositionQuarterNotes() != 0.0)
    //     {
    //         setPosition(0); // real 2kxl doesn't do this
    //     }
    //
    //     return;
    // }
    //
    // playedStepRepetitions = 0;
    // sequencer.setNextSq(NoSequenceIndex);
    //
    // const auto activeSequence = sequencer.getSelectedSequence();
    // const auto pos = getTickPosition();
    //
    // int64_t newTickPosition = pos;
    //
    // if (pos > activeSequence->getLastTick())
    // {
    //     newTickPosition = activeSequence->getLastTick();
    // }
    //
    // recording = false;
    // overdubbing = false;
    //
    // if (countingIn)
    // {
    //     newTickPosition = countInStartPos;
    //     resetCountInPositions();
    //     countingIn = false;
    // }
    //
    // setPosition(Sequencer::ticksToQuarterNotes(newTickPosition));
    //
    // const auto songScreen =
    // sequencer.getScreens()->get<ScreenId::SongScreen>();
    //
    // if (endOfSong)
    // {
    //     songScreen->setOffset(songScreen->getOffset() + 1);
    // }
    //
    // const auto vmpcDirectToDiskRecorderScreen =
    //     sequencer.getScreens()->get<ScreenId::VmpcDirectToDiskRecorderScreen>();
    //
    // if (bouncing && vmpcDirectToDiskRecorderScreen->getRecord() != 4)
    // {
    //     sequencer.stopBouncing();
    // }
    //
    // sequencer.hardware->getLed(hardware::ComponentId::PLAY_LED)
    //     ->setEnabled(false);
    // sequencer.hardware->getLed(hardware::ComponentId::REC_LED)
    //     ->setEnabled(false);
    // sequencer.hardware->getLed(hardware::ComponentId::OVERDUB_LED)
    //     ->setEnabled(false);
}

void NonRtSequencerStateManager::applyInsertBars(const InsertBars &m) noexcept
{
    constexpr bool autoRefreshPlaybackState = false;

    const auto seq = activeState.sequences[m.sequenceIndex];
    const NonRtSequenceStateView seqView(seq);
    const auto oldLastBarIndex = seqView.getLastBarIndex();
    const bool isAppending = m.afterBar - 1 == oldLastBarIndex;

    auto barCountToUse = m.barCount;

    if (oldLastBarIndex + barCountToUse > Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
    {
        barCountToUse = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX - oldLastBarIndex;
    }

    if (barCountToUse == 0)
    {
        return;
    }

    const auto &oldTs = seqView.getTimeSignatures();

    const auto newLastBarIndex = oldLastBarIndex + barCountToUse;

    myApplyMessage(SetLastBarIndex{m.sequenceIndex, BarIndex(newLastBarIndex)},
                   autoRefreshPlaybackState);

    std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> newTs{};

    int out = 0;

    for (int i = 0; i <= oldLastBarIndex; ++i)
    {
        if (i == m.afterBar)
        {
            for (int j = 0;
                 j < barCountToUse && out < Mpc2000XlSpecs::MAX_BAR_COUNT; ++j)
            {
                newTs[out++] =
                    TimeSignature{TimeSigNumerator(4), TimeSigDenominator(4)};
            }
        }
        if (out < Mpc2000XlSpecs::MAX_BAR_COUNT)
        {
            newTs[out++] = oldTs[i];
        }
    }

    for (; out < Mpc2000XlSpecs::MAX_BAR_COUNT; ++out)
    {
        newTs[out] = TimeSignature{};
    }

    myApplyMessage(UpdateTimeSignatures{m.sequenceIndex, newTs},
                   autoRefreshPlaybackState);

    int barStart = 0;

    for (int i = 0; i < m.afterBar; ++i)
    {
        barStart += seqView.getBarLength(i);
    }

    if (!isAppending)
    {
        int newBarStart = 0;
        for (int i = 0; i < m.afterBar + barCountToUse; ++i)
        {
            newBarStart += seqView.getBarLength(i);
        }

        // TODO Tempo change track is excluded. We should double-check
        // if that is the desired behaviour.
        for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
        {
            for (auto &e : seq.tracks[i].events)
            {
                if (e.tick >= barStart)
                {
                    const Tick newTick = e.tick + (newBarStart - barStart);
                    myApplyMessage(UpdateEventTick{e, newTick},
                                   autoRefreshPlaybackState);
                }
            }
        }
    }

    actions.push_back(
        [a = m.onComplete, newLastBarIndex]
        {
            a(newLastBarIndex);
        });

    myApplyMessage(RefreshPlaybackStateWhileNotPlaying{});
}

void NonRtSequencerStateManager::applyCopyEvents(const CopyEvents &m) noexcept
{
    constexpr bool autoRefreshPlaybackState = false;

    const auto segLength = m.sourceEndTick - m.sourceStartTick;
    const auto destOffset = m.destStartTick - m.sourceStartTick;

    auto destNumerator = -1;
    auto destDenominator = -1;
    auto destBarLength = -1;

    assert(m.sourceSequenceIndex >= 0 ||
           m.sourceSequenceIndex == SelectedSequenceIndex);

    const SequenceIndex sourceSequenceIndexToUse =
        m.sourceSequenceIndex >= MinSequenceIndex
            ? m.sourceSequenceIndex
            : activeState.selectedSequenceIndex;

    auto &destSeq = activeState.sequences[m.destSequenceIndex];
    const NonRtSequenceStateView destSeqView(destSeq);
    auto &destTrack = destSeq.tracks[m.destTrackIndex];

    const NonRtSequenceStateView destSequenceView(destSeq);

    const auto destSequenceBarCount = destSeq.lastBarIndex + 1;

    for (int i = 0; i < destSequenceBarCount; i++)
    {
        const auto firstTickOfBar =
            destSequenceView.getFirstTickOfBar(BarIndex(i));

        if (const auto barLength = destSequenceView.getBarLength(i);
            m.destStartTick >= firstTickOfBar &&
            m.destStartTick <= firstTickOfBar + barLength)
        {
            const auto ts = destSequenceView.getTimeSignature(i);
            destNumerator = ts.numerator;
            destDenominator = ts.denominator;
            destBarLength = barLength;
            break;
        }
    }

    const auto minimumRequiredNewSequenceLength = m.destStartTick + segLength;

    const auto ticksToAdd =
        minimumRequiredNewSequenceLength - destSequenceView.getLastTick();

    const auto barsToAdd =
        static_cast<int>(ceil(static_cast<float>(ticksToAdd) / destBarLength));

    const auto initialLastBarIndex = destSequenceView.getLastBarIndex();

    for (int i = 0; i < barsToAdd; i++)
    {
        const auto afterBar = initialLastBarIndex + i + 1;

        if (afterBar >= 998)
        {
            break;
        }

        myApplyMessage(InsertBars{m.destSequenceIndex, 1, afterBar},
                       autoRefreshPlaybackState);
        const TimeSignature ts{TimeSigNumerator(destNumerator),
                               TimeSigDenominator(destDenominator)};
        myApplyMessage(UpdateTimeSignature{m.destSequenceIndex, afterBar, ts},
                       autoRefreshPlaybackState);
    }

    if (!m.copyModeMerge)
    {
        auto &destTrackEvents = destTrack.events;

        for (auto it = destTrackEvents.begin(); it != destTrackEvents.end();)
        {
            if (it->tick >= destOffset &&
                it->tick < destOffset + segLength * m.copyCount)
            {
                destTrackEvents.erase(it);
                continue;
            }
            ++it;
        }
    }

    const auto &sourceSeq = activeState.sequences[sourceSequenceIndexToUse];
    const auto sourceTrackEvents = sourceSeq.tracks[m.sourceTrackIndex].events;

    for (auto &e : sourceTrackEvents)
    {
        if (e.type == EventType::NoteOn &&
            !m.sourceNoteRange.contains(e.noteNumber))
        {
            continue;
        }

        if (e.tick >= m.sourceEndTick)
        {
            break;
        }

        if (e.tick >= m.sourceStartTick)
        {
            for (int copyIndex = 0; copyIndex < m.copyCount; copyIndex++)
            {
                const int tickCandidate =
                    e.tick + destOffset + copyIndex * segLength;

                if (tickCandidate >= destSeqView.getLastTick())
                {
                    break;
                }

                EventState eventCopy = e;
                eventCopy.eventId = m.generateEventId();
                eventCopy.sequenceIndex = m.destSequenceIndex;
                eventCopy.trackIndex = m.destTrackIndex;
                eventCopy.tick = tickCandidate;
                myApplyMessage(InsertEvent{eventCopy},
                               autoRefreshPlaybackState);
            }
        }
    }

    myApplyMessage(SetSelectedSequenceIndex{m.destSequenceIndex});
}
