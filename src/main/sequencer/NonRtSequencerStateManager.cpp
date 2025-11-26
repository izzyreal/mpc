#include "sequencer/NonRtSequencerStateManager.hpp"

#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "Song.hpp"
#include "Step.hpp"
#include "Transport.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
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
    std::visit(
        [&](auto &&m)
        {
            using T = std::decay_t<decltype(m)>;
            if constexpr (std::is_same_v<T, RequestRefreshPlaybackState>)
            {
                TimeInSamples timeToUse = m.timeInSamples;

                if (m.timeInSamples == CurrentTimeInSamples)
                {
                    const auto seq = sequencer->getSelectedSequence();
                    const auto pos =
                        sequencer->getTransport()->getTickPosition();
                    const auto newTimeInSamples =
                        static_cast<TimeInSamples>(SeqUtil::sequenceFrameLength(
                            seq.get(), 0, pos, getSampleRate()));
                }
                else if (m.timeInSamples == NoTimeInSamples)
                {
                    throw std::invalid_argument("Invalid TimeInSamples");
                }
                worker->refreshPlaybackState(timeToUse);
            }
            else if constexpr (std::is_same_v<T, UpdatePlaybackState>)
            {
                activeState.playbackState = m.playbackState;
            }
            else if constexpr (std::is_same_v<T, UpdateTrackIndexOfAllEvents>)
            {
                for (auto &e : activeState.sequences[m.sequence]
                                   .tracks[m.trackIndex]
                                   .events)
                {
                    e.trackIndex = m.trackIndex;
                }
            }
            else if constexpr (std::is_same_v<T, FinalizeNonLiveNoteEvent>)
            {
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
                activeState.sequences[m.sequence]
                    .tracks[m.track]
                    .events.clear();
            }
            else if constexpr (std::is_same_v<T, RemoveDoubles>)
            {
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
                activeState.transportState.positionQuarterNotes =
                    m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T,
                                              SetPlayStartPositionQuarterNotes>)
            {
                activeState.transportState.playStartPositionQuarterNotes =
                    m.positionQuarterNotes;
            }
            else if constexpr (std::is_same_v<T, BumpPositionByTicks>)
            {
                const double delta = Sequencer::ticksToQuarterNotes(m.ticks);
                activeState.transportState.positionQuarterNotes += delta;
            }
            else if constexpr (std::is_same_v<T, SwitchToNextSequence>)
            {
                constexpr bool setPositionTo0 = false;
                enqueue(
                    SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
                enqueue(Stop{});
                constexpr bool fromStart = true;
                enqueue(Play{fromStart});
            }
            else if constexpr (std::is_same_v<T, SetSelectedSequenceIndex>)
            {
                activeState.selectedSequenceIndex = m.sequenceIndex;

                if (m.setPositionTo0)
                {
                    activeState.transportState.positionQuarterNotes = 0;
                }
            }
            else if constexpr (std::is_same_v<T, Stop>)
            {
                worker->getSequencer()->getTransport()->stop();
            }
            else if constexpr (std::is_same_v<T, Play>)
            {
                applyPlayMessage(m.fromStart);
            }
            else if constexpr (std::is_same_v<T, UpdateBarLength>)
            {
                activeState.sequences[m.sequenceIndex].barLengths[m.barIndex] = m.length;
            }
            else if constexpr (std::is_same_v<T, UpdateBarLengths>)
            {
                activeState.sequences[m.sequenceIndex].barLengths = m.barLengths;
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignatures>)
            {
                activeState.sequences[m.sequenceIndex].timeSignatures = m.timeSignatures;
            }
            else if constexpr (std::is_same_v<T, UpdateTimeSignature>)
            {
                activeState.sequences[m.sequenceIndex].timeSignatures[m.barIndex] = m.timeSignature;
                activeState.sequences[m.sequenceIndex].barLengths[m.barIndex] = m.timeSignature.getBarLength();
            }
            else if constexpr (std::is_same_v<T, UpdateEvents>)
            {
                activeState.sequences[m.sequence].tracks[m.track].events = m.eventStates;
            }
        },
        msg);
}

void NonRtSequencerStateManager::applyPlayMessage(
    const bool fromStart) const noexcept
{
    const auto transport = worker->getSequencer()->getTransport();

    if (transport->isPlaying())
    {
        return;
    }

    transport->setEndOfSong(false);

    const auto songScreen = worker->getSequencer()
                                ->getScreens()
                                ->get<lcdgui::ScreenId::SongScreen>();

    const auto currentSong =
        worker->getSequencer()->getSong(songScreen->getSelectedSongIndex());

    const bool songMode = worker->getSequencer()->isSongModeEnabled();

    if (songMode)
    {
        if (!currentSong->isUsed())
        {
            return;
        }

        if (fromStart)
        {
            const int oldSongSequenceIndex =
                worker->getSequencer()->getSongSequenceIndex();
            songScreen->setOffset(-1);
            if (worker->getSequencer()->getSongSequenceIndex() !=
                oldSongSequenceIndex)
            {
                worker->getSequencer()->setSelectedSequenceIndex(
                    worker->getSequencer()->getSongSequenceIndex(), true);
            }
        }

        if (songScreen->getOffset() + 1 > currentSong->getStepCount() - 1)
        {
            return;
        }

        int step = songScreen->getOffset() + 1;

        if (step > currentSong->getStepCount())
        {
            step = currentSong->getStepCount() - 1;
        }

        if (const std::shared_ptr<Step> currentStep =
                currentSong->getStep(step).lock();
            !worker->getSequencer()
                 ->getSequence(currentStep->getSequence())
                 ->isUsed())
        {
            return;
        }
    }

    const auto countMetronomeScreen =
        worker->getSequencer()
            ->getScreens()
            ->get<lcdgui::ScreenId::CountMetronomeScreen>();

    const auto countInMode = countMetronomeScreen->getCountInMode();

    std::optional<int64_t> positionQuarterNotesToStartPlayingFrom =
        std::nullopt;

    if (!transport->isCountEnabled() || countInMode == 0 ||
        (countInMode == 1 && !transport->isRecordingOrOverdubbing()))
    {
        if (fromStart && activeState.transportState.positionQuarterNotes != 0)
        {
            positionQuarterNotesToStartPlayingFrom = 0;
        }
    }

    const auto activeSequence = worker->getSequencer()->getSelectedSequence();

    if (transport->isCountEnabled() && !songMode)
    {
        if (countInMode == 2 ||
            (countInMode == 1 && transport->isRecordingOrOverdubbing()))
        {
            if (fromStart)
            {
                positionQuarterNotesToStartPlayingFrom =
                    Sequencer::ticksToQuarterNotes(
                        activeSequence->getLoopStart());
            }
            else
            {
                positionQuarterNotesToStartPlayingFrom =
                    Sequencer::ticksToQuarterNotes(
                        activeSequence->getFirstTickOfBar(
                            transport->getCurrentBarIndex()));
            }

            transport->setCountInStartPosTicks(Sequencer::quarterNotesToTicks(
                activeState.transportState.positionQuarterNotes));

            transport->setCountInEndPosTicks(activeSequence->getLastTickOfBar(
                transport->getCurrentBarIndex()));
            transport->setCountingIn(true);
        }
    }

    if (positionQuarterNotesToStartPlayingFrom)
    {
        transport->setPosition(*positionQuarterNotesToStartPlayingFrom);
    }

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
            worker->getSequencer()->storeSelectedSequenceInUndoPlaceHolder();
        }
    }

    if (worker->getSequencer()->isBouncePrepared())
    {
        worker->getSequencer()->startBouncing();
    }
    else
    {
        transport->getSequencerPlaybackEngine()->start();
    }
}
