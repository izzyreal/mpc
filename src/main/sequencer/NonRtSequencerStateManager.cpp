#include "sequencer/NonRtSequencerStateManager.hpp"

#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
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

void NonRtSequencerStateManager::applyMessage(
    const NonRtSequencerMessage &msg) noexcept
{
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, RefreshPlaybackStateWhileNotPlaying>)
            {
                worker->refreshPlaybackState(activeState.transport.positionQuarterNotes, 0, m.onComplete);
            }
            else if constexpr (std::is_same_v<T, RefreshPlaybackStateWhilePlaying>)
            {
                worker->refreshPlaybackState(activeState.transport.positionQuarterNotes, CurrentTimeInSamples, m.onComplete);
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
                enqueue(Stop{});
                enqueue(PlayFromStart{});
            }
            else if constexpr (std::is_same_v<T, SetSelectedSequenceIndex>)
            {
                // printf("Applying SetSelectedSequenceIndex\n");
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
                    auto onComplete = [this]{enqueue(Play{});};
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
                activeState.sequences[m.sequenceIndex].timeSignatures =
                    m.timeSignatures;
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignature>)
            {
                // printf("Applying UpdateTimeSignature\n");
                activeState.sequences[m.sequenceIndex]
                    .timeSignatures[m.barIndex] = m.timeSignature;
                activeState.sequences[m.sequenceIndex].barLengths[m.barIndex] =
                    m.timeSignature.getBarLength();
            }
            else if constexpr (std::is_same_v<T, UpdateEvents>)
            {
                printf("Applying UpdateEvents\n");
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

    if (sequencer->getTransport()->isPlaying()  && isVariantAnyOf(
            msg, MessagesThatInvalidPlaybackStateWhilePlaying{}))
    {
        publishState();
        applyMessage(RefreshPlaybackStateWhilePlaying{});
    }
    else if (!sequencer->getTransport()->isPlaying()  && isVariantAnyOf(
            msg, MessagesThatInvalidPlaybackStateWhileNotPlaying{}))
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
    //         const int oldSongSequenceIndex = sequencer->getSongSequenceIndex();
    //         songScreen->setOffset(-1);
    //         if (sequencer->getSongSequenceIndex() != oldSongSequenceIndex)
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
    //     if (fromStart && activeState.transportState.positionQuarterNotes != 0)
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
        //     if (*positionQuarterNotesToStartPlayingFrom != activeState.transportState.positionQuarterNotes)
        //     {
        //         activeState.transportState.positionQuarterNotes = *positionQuarterNotesToStartPlayingFrom;
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
    //     if (const auto snapshot = sequencer.getNonRtStateManager()->getSnapshot();
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
    // const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();
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
