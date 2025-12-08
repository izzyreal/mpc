#include "Sequencer.hpp"

#include "MpcSpecs.hpp"

#include "sequencer/Transport.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Song.hpp"

#include "sampler/Sampler.hpp"

#include "engine/Voice.hpp"

#include "performance/PerformanceManager.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/ScreenIdGroups.hpp"

#include "StrUtil.hpp"
#include "SequencerStateManager.hpp"

#include <chrono>

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::engine;
using namespace mpc::performance;
using namespace mpc::sampler;
using namespace mpc::audiomidi;

uint32_t Sequencer::quarterNotesToTicks(const double quarterNotes)
{
    return static_cast<uint32_t>(
        std::floor(quarterNotes * static_cast<double>(TICKS_PER_QUARTER_NOTE)));
}

double Sequencer::ticksToQuarterNotes(const double ticks)
{
    return ticks / static_cast<double>(TICKS_PER_QUARTER_NOTE);
}

uint64_t currentTimeMillis()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

Sequencer::Sequencer(
    const utils::PostToAudioThreadFn &postToAudioThread,
    const std::function<void()> &flushMidiNoteOffs,
    const std::shared_ptr<Clock> &clock,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    const std::function<std::shared_ptr<Screens>()> &getScreens,
    std::vector<std::shared_ptr<Voice>> *voices,
    const std::function<bool()> &isAudioServerRunning,
    const std::shared_ptr<hardware::Hardware> &hardware,
    const std::function<bool()> &isBouncePrepared,
    const std::function<void()> &startBouncing,
    const std::function<void()> &stopBouncing,
    const std::function<bool()> &isBouncing,
    const std::function<bool()> &isEraseButtonPressed,
    const std::shared_ptr<PerformanceManager> &performanceManager,
    const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<EventHandler> &eventHandler,
    const std::function<bool()> &isSixteenLevelsEnabled)
    : getScreens(getScreens), isBouncePrepared(isBouncePrepared),
      startBouncing(startBouncing), hardware(hardware), isBouncing(isBouncing),
      stopBouncing(stopBouncing), layeredScreen(layeredScreen), clock(clock),
      flushMidiNoteOffs(flushMidiNoteOffs),
      postToAudioThread(postToAudioThread), voices(voices),
      isAudioServerRunning(isAudioServerRunning),
      isEraseButtonPressed(isEraseButtonPressed),
      performanceManager(performanceManager), sampler(sampler),
      eventHandler(eventHandler), isSixteenLevelsEnabled(isSixteenLevelsEnabled)
{
    stateManager = std::make_shared<SequencerStateManager>(this);
}

Sequencer::~Sequencer() {}

void Sequencer::playTick(const Tick tick) const
{
    if (transport->isMetronomeOnlyEnabled())
    {
        return;
    }

    const auto seqIndex = isSongModeEnabled() ? getSongSequenceIndex()
                                              : getSelectedSequenceIndex();
    auto seq = sequences[seqIndex].get();
    const auto secondSequenceScreen =
        getScreens()->get<ScreenId::SecondSeqScreen>();

    for (int i = 0; i < 2; i++)
    {
        if (i == 1)
        {
            if (!secondSequenceEnabled.load() ||
                secondSequenceScreen->sq ==
                    seqIndex) // Real 2KXL would play all events twice (i.e.
                              // double as loud as normal) for the last clause
            {
                break;
            }

            seq = sequences[secondSequenceScreen->sq].get();

            if (!seq->isUsed())
            {
                break;
            }
        }

        stateManager->processLiveNoteEventRecordingQueues(
            tick, *seq->getSnapshot(seq->getSequenceIndex()));

        for (const auto &track : seq->getTracks())
        {
            while (track->getNextTick() == tick)
            {
                track->playNext();
            }
        }
    }
}

SequenceIndex Sequencer::getSelectedSequenceIndex() const
{
    return stateManager->getSnapshot().getSelectedSequenceIndex();
}

std::shared_ptr<Track> Sequencer::getSelectedTrack()
{
    if (!getSelectedSequence())
    {
        return {};
    }

    return getSelectedSequence()->getTrack(selectedTrackIndex);
}

template <typename T>
std::shared_ptr<T> Sequencer::getBus(const BusType busType) const
{
    auto result = std::dynamic_pointer_cast<T>(buses[busTypeToIndex(busType)]);
    return result;
}

std::shared_ptr<Bus> Sequencer::getBus(const BusType busType) const
{
    return buses[busTypeToIndex(busType)];
}

std::shared_ptr<DrumBus> Sequencer::getDrumBus(const BusType busType) const
{
    assert(isDrumBusType(busType));
    auto result = getBus<DrumBus>(busType);
    assert(result);
    return result;
}

std::shared_ptr<DrumBus>
Sequencer::getDrumBus(const DrumBusIndex drumBusIndex) const
{
    assert(drumBusIndex.get() >= 0 &&
           drumBusIndex.get() <= Mpc2000XlSpecs::DRUM_BUS_COUNT - 1);
    auto result = getBus<DrumBus>(drumBusIndexToDrumBusType(drumBusIndex));
    assert(result);
    return result;
}

std::shared_ptr<TempoChangeEvent> Sequencer::getCurrentTempoChangeEvent()
{
    auto index = -1;
    const auto s = getSelectedSequence();

    if (!s->isUsed())
    {
        return {};
    }

    for (const auto &tce : s->getTempoChangeEvents())
    {
        if (transport->getTickPosition() >= tce->getTick())
        {
            index++;
        }
        else
        {
            break;
        }
    }

    if (index == -1)
    {
        return {};
    }

    return s->getTempoChangeEvents()[index];
}

std::shared_ptr<EventHandler> Sequencer::getEventHandler()
{
    return eventHandler;
}

void Sequencer::copyTrack(const std::shared_ptr<Track> &src,
                          const std::shared_ptr<Track> &dest) const
{
    dest->setEventStates(src->getEventStates());
    copyTrackParameters(src, dest);
}

void Sequencer::makeNewSequence(const SequenceIndex sequenceIndex)
{
    makeNewSequence(sequences[sequenceIndex]);
    sequences[sequenceIndex]->setSequenceIndex(sequenceIndex);
}

void Sequencer::makeNewSequence(std::shared_ptr<Sequence> &destination)
{
    const std::function dispatch = [this](SequencerMessage &&m)
    {
        stateManager->enqueue(std::move(m));
    };

    const std::function getSnapshot = [this](const SequenceIndex sequenceIndex)
    {
        return stateManager->getSnapshot().getSequenceState(sequenceIndex);
    };

    destination = std::make_shared<Sequence>(
        layeredScreen->postToUiThread, stateManager, getSnapshot, dispatch,
        [&](const int trackIndex)
        {
            return defaultTrackNames[trackIndex];
        },
        [&]
        {
            return transport->getTickPosition();
        },
        getScreens,
        [&]
        {
            return isRecordingModeMulti();
        },
        [&]
        {
            return getSelectedSequence();
        },
        [&]
        {
            return transport->getAutoPunchMode();
        },
        [&](const BusType busType)
        {
            return getBus<Bus>(busType);
        },
        [&]
        {
            return isEraseButtonPressed();
        },
        [&](const ProgramPadIndex programPadIndex,
            const ProgramIndex programIndex)
        {
            return performanceManager.lock()->getSnapshot().isProgramPadPressed(
                programPadIndex, programIndex);
        },
        sampler, eventHandler,
        [&]
        {
            return isSixteenLevelsEnabled();
        },
        [&]
        {
            return getSelectedTrackIndex();
        },
        [&]
        {
            return transport->isRecording();
        },
        [&]
        {
            return transport->isOverdubbing();
        },
        [&]
        {
            return transport->isPunchEnabled();
        },
        [&]
        {
            return transport->getPunchInTime();
        },
        [&]
        {
            return transport->getPunchOutTime();
        },
        [&]
        {
            return isSoloEnabled();
        },
        [&]
        {
            return transport->getCurrentBarIndex();
        });
}

void Sequencer::copySequenceParameters(const std::shared_ptr<Sequence> &source,
                                       const SequenceIndex destIndex) const
{
    const auto dest = sequences[destIndex];

    dest->setSequenceIndex(destIndex);
    dest->setName(source->getName());
    dest->setLoopEnabled(source->isLoopEnabled());
    dest->setUsed(source->isUsed());
    dest->setDeviceNames(source->getDeviceNames());
    dest->setInitialTempo(source->getInitialTempo());
    dest->setBarLengths(source->getBarLengths());
    dest->setTimeSignatures(source->getTimeSignatures());
    dest->setFirstLoopBarIndex(source->getFirstLoopBarIndex());
    dest->setLastLoopBarIndex(source->getLastLoopBarIndex());
    dest->setTempoChangeOn(source->isTempoChangeOn());
}

void Sequencer::copyTrackParameters(const std::shared_ptr<Track> &source,
                                    const std::shared_ptr<Track> &dest) const
{
    dest->setOn(source->isOn());
    dest->setDeviceIndex(source->getDeviceIndex());
    dest->setBusType(source->getBusType());
    dest->setVelocityRatio(source->getVelocityRatio());
    dest->setProgramChange(source->getProgramChange());
    dest->setName(source->getName());
    stateManager->enqueue(SetTrackUsed{dest->getSequenceIndex(),
                                       dest->getIndex(), source->isUsed()});
}

std::shared_ptr<SequencerStateManager> Sequencer::getStateManager() const
{
    return stateManager;
}

std::shared_ptr<Transport> Sequencer::getTransport()
{
    return transport;
}

void Sequencer::init()
{
    transport = std::make_shared<Transport>(*this);

    for (int midiBusIndex = 0; midiBusIndex < Mpc2000XlSpecs::MIDI_BUS_COUNT;
         ++midiBusIndex)
    {
        buses.emplace_back(std::make_shared<MidiBus>());
    }

    for (int drumBusIndex = 0; drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT;
         ++drumBusIndex)
    {
        buses.emplace_back(std::make_shared<DrumBus>(
            DrumBusIndex(drumBusIndex), performanceManager.lock()));
    }

    lastTap = currentTimeMillis();
    nextSq = NoSequenceIndex;

    const auto userScreen = getScreens()->get<ScreenId::UserScreen>();
    defaultSequenceName = StrUtil::trim(userScreen->sequenceName);

    for (int i = 0; i < 64; i++)
    {
        std::string name = "Track-";
        name = name.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
        defaultTrackNames.push_back(name);
    }

    selectedTrackIndex = 0;

    recordingModeMulti = userScreen->recordingModeMulti;

    soloEnabled = false;

    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT; ++i)
    {
        makeNewSequence(SequenceIndex(i));
    }

    const auto dispatch = [manager = stateManager](SongMessage &&m)
    {
        manager->enqueue(m);
    };

    for (int i = 0; i < 20; i++)
    {
        const auto getSnapshot = [manager = stateManager, idx = SongIndex(i)]
        {
            return manager->getSnapshot().getSongStateView(idx);
        };

        songs[i] = std::make_shared<Song>(SongIndex(i), getSnapshot, dispatch);
    }
}

bool Sequencer::isSoloEnabled() const
{
    return soloEnabled;
}

void Sequencer::setSoloEnabled(const bool b)
{
    if (soloEnabled == b)
    {
        return;
    }

    soloEnabled = b;

    if (soloEnabled)
    {
        for (int drumIndex = 0; drumIndex < 4; ++drumIndex)
        {
            for (int note = 35; note <= 98; note++)
            {
                for (const auto &voice : *voices)
                {
                    if (!voice->isFinished() && voice->getNote() == note &&
                        voice->getVoiceOverlapMode() ==
                            VoiceOverlapMode::NOTE_OFF &&
                        !voice->isDecaying() &&
                        drumIndex == voice->getMuteInfo().getDrum())
                    {
                        voice->startDecay();
                        break;
                    }
                }
            }
        }
    }
}

std::shared_ptr<Sequence> Sequencer::getSequence(const int i)
{
    return sequences[i];
}

std::string Sequencer::getDefaultSequenceName()
{
    return defaultSequenceName;
}

void Sequencer::setDefaultSequenceName(const std::string &s)
{
    defaultSequenceName = s;
}

void Sequencer::setSelectedSequenceIndex(const SequenceIndex sequenceIndexToUse,
                                         const bool shouldSetPositionTo0)
{
    if (!transport->isPlaying() &&
        sequenceIndexToUse != getSelectedSequenceIndex())
    {
        resetUndo();
    }

    stateManager->enqueue(SetSelectedSequenceIndex{
        std::clamp(sequenceIndexToUse, MinSequenceIndex, MaxSequenceIndex),
        shouldSetPositionTo0});
}

void Sequencer::setTimeDisplayStyle(const int i)
{
    timeDisplayStyle = i;
}

int Sequencer::getTimeDisplayStyle() const
{
    return timeDisplayStyle;
}

void Sequencer::setRecordingModeMulti(const bool b)
{
    recordingModeMulti = b;
}

bool Sequencer::isRecordingModeMulti() const
{
    return recordingModeMulti;
}

int Sequencer::getSelectedTrackIndex() const
{
    return selectedTrackIndex;
}

void Sequencer::undoSeq()
{
    const auto selectedSequence = getSelectedSequence();
    const auto selectedSequenceIndex = selectedSequence->getSequenceIndex();

    if (transport->isPlaying() || !sequences[UndoSequenceIndex]->isUsed() ||
        !selectedSequence)
    {
        return;
    }

    postToAudioThread(utils::Task(
        [this, selectedSequence, selectedSequenceIndex]
        {
            copySequence(selectedSequence, TempSequenceIndex);
            stateManager->drainQueue();
            copySequence(sequences[UndoSequenceIndex], selectedSequenceIndex);
            stateManager->drainQueue();
            copySequence(sequences[TempSequenceIndex], UndoSequenceIndex);
            undoSeqAvailable = !undoSeqAvailable;
            hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
                ->setEnabled(undoSeqAvailable);
        }));
}

void Sequencer::deleteAllSequences()
{
    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT; ++i)
    {
        deleteSequence(i);
    }

    setSelectedSequenceIndex(MinSequenceIndex, true);
}

void Sequencer::deleteSequence(const int i) const
{
    sequences[i]->init(0);
    sequences[i]->setUsed(false);
    std::string res = defaultSequenceName;
    res.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
    sequences[i]->setName(res);
    stateManager->enqueue(DeleteSequence{SequenceIndex(i)});
}

void Sequencer::copySequence(const std::shared_ptr<Sequence> &source,
                             const SequenceIndex destIndex) const
{
    sequences[destIndex]->init(source->getLastBarIndex());

    copySequenceParameters(source, destIndex);

    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; i++)
    {
        copyTrack(source->getTrack(i), sequences[destIndex]->getTrack(i));
    }
}

void Sequencer::copySong(const int source, const int dest) const
{
    if (source == dest)
    {
        return;
    }

    const auto s0 = songs[source];

    if (!s0->isUsed())
    {
        return;
    }

    deleteSong(dest);

    const auto s1 = songs[dest];

    s1->setUsed(true);

    for (int stepIndex = 0; stepIndex < s0->getStepCount(); ++stepIndex)
    {
        const auto sourceStep = s0->getStep(SongStepIndex(stepIndex));
        s1->insertStep(SongStepIndex(stepIndex));
        s1->setStepRepetitionCount(SongStepIndex(stepIndex),
                                   sourceStep.repetitionCount);
        s1->setStepSequenceIndex(SongStepIndex(stepIndex),
                                 sourceStep.sequenceIndex);
    }

    s1->setFirstLoopStepIndex(s0->getFirstLoopStepIndex());
    s1->setLastLoopStepIndex(s0->getLastLoopStepIndex());
    s1->setName(s0->getName());
    s1->setLoopEnabled(s0->isLoopEnabled());
}

void Sequencer::copyTrack(const int sourceTrackIndex, const int destTrackIndex,
                          const int sourceSequenceIndex,
                          const int destSequenceIndex) const
{
    if (sourceSequenceIndex == destSequenceIndex &&
        sourceTrackIndex == destTrackIndex)
    {
        return;
    }

    const auto src = sequences[sourceSequenceIndex]->getTrack(sourceTrackIndex);
    const auto dest = sequences[destSequenceIndex]->getTrack(destTrackIndex);
    copyTrack(src, dest);
}

std::vector<std::string> &Sequencer::getDefaultTrackNames()
{
    return defaultTrackNames;
}

std::string Sequencer::getDefaultTrackName(const int i)
{
    return defaultTrackNames[i];
}

void Sequencer::setDefaultTrackName(const std::string &s, const int i)
{
    defaultTrackNames[i] = s;
}

std::shared_ptr<Sequence> Sequencer::getSelectedSequence()
{
    if (const bool songMode = isSongModeEnabled();
        songMode && songs[getSelectedSongIndex()]->getStepCount() != 0)
    {
        return sequences[getSongSequenceIndex() >= 0
                             ? getSongSequenceIndex()
                             : getSelectedSequenceIndex()];
    }

    return sequences[getSelectedSequenceIndex()];
}

int Sequencer::getUsedSequenceCount() const
{
    return getUsedSequences().size();
}

std::vector<std::shared_ptr<Sequence>> Sequencer::getUsedSequences() const
{
    std::vector<std::shared_ptr<Sequence>> usedSeqs;

    for (const auto &s : sequences)
    {
        if (s->isUsed())
        {
            usedSeqs.push_back(s);
        }
    }

    return usedSeqs;
}

std::vector<int> Sequencer::getUsedSequenceIndexes() const
{
    std::vector<int> usedSeqs;

    for (int i = 0; i < 99; i++)
    {
        if (const auto s = sequences[i]; s->isUsed())
        {
            usedSeqs.push_back(i);
        }
    }

    return usedSeqs;
}

void Sequencer::goToPreviousEvent()
{
    const auto s = getSelectedSequence();
    const auto t = s->getTrack(getSelectedTrackIndex());

    auto newPos = 0;
    auto events = t->getEvents();
    std::reverse(events.begin(), events.end());

    for (const auto &e : events)
    {
        if (e->getTick() < transport->getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    transport->setPosition(ticksToQuarterNotes(newPos));
}

void Sequencer::goToNextEvent()
{
    const auto s = getSelectedSequence();
    const auto t = s->getTrack(getSelectedTrackIndex());

    auto newPos = s->getLastTick();

    for (const auto &e : t->getEvents())
    {
        if (e->getTick() > transport->getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    transport->setPosition(ticksToQuarterNotes(newPos));
}

void Sequencer::goToPreviousStep()
{
    const auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();

    const auto stepSize = timingCorrectScreen->getNoteValueLengthInTicks();
    const auto pos = transport->getTickPosition();
    const auto stepCount =
        static_cast<int>(
            ceil(getSelectedSequence()->getLastTick() / stepSize)) +
        1;
    std::vector<int> stepGrid(stepCount);

    for (int i = 0; i < stepGrid.size(); i++)
    {
        stepGrid[i] = i * stepSize;
    }

    auto prevStepIndex = 0;

    for (const auto l : stepGrid)
    {
        if (pos <= l)
        {
            break;
        }

        prevStepIndex++;
    }

    prevStepIndex--;

    if (prevStepIndex < 0)
    {
        prevStepIndex = 0;
    }

    transport->setPosition(ticksToQuarterNotes(prevStepIndex * stepSize));
}

void Sequencer::goToNextStep()
{
    const auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();

    const auto stepSize = timingCorrectScreen->getNoteValueLengthInTicks();
    const auto pos = transport->getTickPosition();

    std::vector<int> stepGrid(
        ceil(getSelectedSequence()->getLastTick() / stepSize));

    for (int i = 0; i < stepGrid.size(); i++)
    {
        stepGrid[i] = i * stepSize;
    }

    auto nextStepIndex = -1;

    for (const auto l : stepGrid)
    {
        if (pos < l)
        {
            break;
        }

        nextStepIndex++;
    }

    nextStepIndex++;

    if (nextStepIndex >= stepGrid.size())
    {
        nextStepIndex = stepGrid.size() - 1;
    }

    transport->setPosition(ticksToQuarterNotes(nextStepIndex * stepSize));
}

void Sequencer::tap()
{
    if (transport->isPlaying())
    {
        return;
    }

    const auto now = currentTimeMillis();

    if (now - lastTap > 2000)
    {
        taps = {0, 0, 0, 0};
        tapIndex = 0;
    }

    lastTap = now;

    taps[tapIndex] = lastTap;

    uint64_t accum = 0;

    int index = tapIndex;

    tapIndex++;

    if (tapIndex > 3)
    {
        tapIndex = 0;
    }

    int usedTapsCounter = 0;

    for (int i = 0; i < 3; i++)
    {
        const auto l0 = taps[index];

        index--;

        if (index < 0)
        {
            index = 3;
        }

        const auto l1 = taps[index];

        if (l0 == 0 || l1 == 0)
        {
            break;
        }

        accum += l0 - l1;
        usedTapsCounter++;
    }

    if (accum == 0)
    {
        return;
    }

    auto newTempo = 60000.0 / (accum / usedTapsCounter);
    newTempo = floor(newTempo * 10) / 10;
    transport->setTempo(newTempo);
}

std::shared_ptr<Sequence> Sequencer::getCurrentlyPlayingSequence()
{
    const auto seqIndex = getCurrentlyPlayingSequenceIndex();

    if (seqIndex == NoSequenceIndex)
    {
        return {};
    }

    return sequences[seqIndex];
}

void Sequencer::setSelectedTrackIndex(const int i)
{
    selectedTrackIndex =
        std::clamp(static_cast<int8_t>(i), static_cast<int8_t>(0),
                   Mpc2000XlSpecs::LAST_TRACK_INDEX);
}

SequenceIndex Sequencer::getCurrentlyPlayingSequenceIndex() const
{
    if (isSongModeEnabled())
    {
        const auto song = songs[getSelectedSongIndex()];

        if (!song->isUsed() || isSelectedSongStepIndexEndOfSong())
        {
            return NoSequenceIndex;
        }

        const auto currentStep = getSelectedSongStepIndex();
        return song->getStep(currentStep).sequenceIndex;
    }

    return getSelectedSequenceIndex();
}

SequenceIndex Sequencer::getNextSq() const
{
    return nextSq;
}

SequenceIndex Sequencer::getFirstUsedSeqDown(const SequenceIndex from,
                                             const bool unused) const
{
    for (int i = from; i >= MinSequenceIndex; i--)
    {
        const auto candidate =
            unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

        if (candidate)
        {
            return SequenceIndex(i);
        }
    }

    return NoSequenceIndex;
}

SequenceIndex Sequencer::getFirstUsedSeqUp(const SequenceIndex from,
                                           const bool unused) const
{
    for (int i = from; i <= MaxSequenceIndex; i++)
    {
        const auto candidate =
            unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

        if (candidate)
        {
            return SequenceIndex(i);
        }
    }

    return NoSequenceIndex;
}

void Sequencer::setNextSq(SequenceIndex i)
{
    if (i < NoSequenceIndex)
    {
        i = NoSequenceIndex;
    }
    if (i > MaxSequenceIndex)
    {
        i = MaxSequenceIndex;
    }

    const auto startingFromScratch = nextSq == NoSequenceIndex;

    auto up = i > nextSq;

    if (startingFromScratch)
    {
        up = i > selectedTrackIndex;
    }

    const auto candidate =
        SequenceIndex(up ? getFirstUsedSeqUp(i) : getFirstUsedSeqDown(i));

    if (up && candidate == NoSequenceIndex)
    {
        return;
    }

    nextSq = candidate;
}

void Sequencer::setNextSqPad(const SequenceIndex i)
{
    if (!sequences[i]->isUsed())
    {
        nextSq = NoSequenceIndex;
        return;
    }

    nextSq = i;
}

std::shared_ptr<Song> Sequencer::getSong(const int i)
{
    return songs[i];
}

std::shared_ptr<Song> Sequencer::getSelectedSong() const
{
    return songs[getSelectedSongIndex()];
}

void Sequencer::deleteSong(const int i) const
{
    stateManager->enqueue(DeleteSong{SongIndex(i)});
}

bool Sequencer::isSongModeEnabled() const
{
    return screengroups::isSongScreen(layeredScreen->getCurrentScreenId());
}

SequenceIndex Sequencer::getSongSequenceIndex() const
{
    if (isSelectedSongStepIndexEndOfSong())
    {
        return NoSequenceIndex;
    }

    const auto song = songs[getSelectedSongIndex()];
    return song->getStep(getSelectedSongStepIndex()).sequenceIndex;
}

bool Sequencer::isSecondSequenceEnabled() const
{
    return secondSequenceEnabled.load();
}

void Sequencer::setSecondSequenceEnabled(const bool b)
{
    secondSequenceEnabled.store(b);
}

void Sequencer::flushTrackNoteCache() const
{
    stateManager->flushNoteCache();
}

void Sequencer::copySelectedSequenceToUndoSequence()
{
    copySequence(getSelectedSequence(), UndoSequenceIndex);

    undoSeqAvailable = true;

    layeredScreen->postToUiThread(utils::Task(
        [this]
        {
            hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
                ->setEnabled(true);
        }));
}

void Sequencer::resetUndo()
{
    undoSeqAvailable = false;
    hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)->setEnabled(false);
}

SongIndex Sequencer::getSelectedSongIndex() const
{
    return stateManager->getSnapshot().getSelectedSongIndex();
}

void Sequencer::setSelectedSongIndex(const SongIndex songIndex) const
{
    const auto song = songs[songIndex];
    const auto sequenceOfFirstStep =
        song->getStepCount() == 0
            ? NoSequenceIndex
            : song->getStep(MinSongStepIndex).sequenceIndex;

    stateManager->enqueue(SetSelectedSongIndex{songIndex, sequenceOfFirstStep});
}

SongStepIndex Sequencer::getSelectedSongStepIndex() const
{
    return stateManager->getSnapshot().getSelectedSongStepIndex();
}

bool Sequencer::isSelectedSongStepIndexEndOfSong() const
{
    const auto stepIndex = getSelectedSongStepIndex();
    const auto song = getSelectedSong();
    const auto stepCount = song->getStepCount();
    return stepIndex >= stepCount;
}

void Sequencer::setSelectedSongStepIndex(
    const SongStepIndex newSongStepIndex) const
{
    const auto song = getSelectedSong();

    const auto lastStepIndex = SongStepIndex(song->getStepCount() - 1);

    const auto stepIndexToUse = std::clamp(
        newSongStepIndex, MinSongStepIndex,
        song->getStepCount() == 0 ? SongStepIndex(0)
                                  : SongStepIndex(lastStepIndex + 1));

    const auto sequenceIndex =
        stepIndexToUse > lastStepIndex
            ? NoSequenceIndex
            : song->getStep(stepIndexToUse).sequenceIndex;

    stateManager->enqueue(
        SetSelectedSongStepIndex{stepIndexToUse, sequenceIndex});
}

template std::shared_ptr<MidiBus> Sequencer::getBus(BusType) const;
template std::shared_ptr<DrumBus> Sequencer::getBus(BusType) const;
