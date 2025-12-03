#include "Sequencer.hpp"

#include "MpcSpecs.hpp"

#include "sequencer/Transport.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"

#include "sampler/Sampler.hpp"

#include "engine/Voice.hpp"

#include "performance/PerformanceManager.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
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
    : flushMidiNoteOffs(flushMidiNoteOffs), getScreens(getScreens), isBouncePrepared(isBouncePrepared),
      startBouncing(startBouncing), hardware(hardware), isBouncing(isBouncing),
      stopBouncing(stopBouncing), layeredScreen(layeredScreen), clock(clock),
      voices(voices), isAudioServerRunning(isAudioServerRunning),
      isEraseButtonPressed(isEraseButtonPressed),
      performanceManager(performanceManager), sampler(sampler),
      eventHandler(eventHandler), isSixteenLevelsEnabled(isSixteenLevelsEnabled)
{
    stateManager = std::make_shared<SequencerStateManager>(this);
}

Sequencer::~Sequencer() {}

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

    purgeAllSequences();

    for (int i = 0; i < 20; i++)
    {
        songs[i] = std::make_shared<Song>();
    }
}

void Sequencer::deleteSong(const int i)
{
    songs[i] = std::make_shared<Song>();
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

void Sequencer::playToTick(const int targetTick) const
{
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
                transport->isMetronomeOnlyEnabled() ||
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

        if (!transport->isMetronomeOnlyEnabled())
        {
            for (const auto &track : seq->getTracks())
            {
                while (track->getNextTick() <= targetTick)
                {
                    track->playNext();
                }
            }
        }

        while (seq->getTempoChangeTrack()->getNextTick() <= targetTick)
        {
            seq->getTempoChangeTrack()->playNext();
        }
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
                                         const bool shouldSetPositionTo0) const
{
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
    if (transport->isPlaying())
    {
        return;
    }

    if (!sequences[UndoSequenceIndex]->isUsed())
    {
        return;
    }

    sequences[UndoSequenceIndex].swap(sequences[getSelectedSequenceIndex()]);
    undoSeqAvailable = !undoSeqAvailable;
    hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
        ->setEnabled(undoSeqAvailable);
}

void Sequencer::setSequence(const SequenceIndex sequenceIndex,
                            std::shared_ptr<Sequence> s)
{
    sequences[sequenceIndex].swap(s);
}

void Sequencer::purgeAllSequences()
{
    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT; i++)
    {
        purgeSequence(i);
    }

    setSelectedSequenceIndex(MinSequenceIndex, true);
}

std::shared_ptr<Sequence>
Sequencer::makeNewSequence(SequenceIndex sequenceIndex)
{
    const std::function dispatch = [this](SequencerMessage &&m)
    {
        stateManager->enqueue(std::move(m));
    };

    const std::function getSnapshot = [this, sequenceIndex]
    {
        return stateManager->getSnapshot().getSequenceState(sequenceIndex);
    };

    return std::make_shared<Sequence>(
        sequenceIndex, stateManager, getSnapshot, dispatch,
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

void Sequencer::purgeSequence(const int i)
{
    sequences[i] = makeNewSequence(SequenceIndex(i));
    std::string res = defaultSequenceName;
    res.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
    sequences[i]->setName(res);
}

void Sequencer::copySequence(const int source, const int destination)
{
    copySequence(sequences[source], SequenceIndex(destination));
}

void Sequencer::copySequenceParameters(const int source, const int dest) const
{
    copySequenceParameters(sequences[source], sequences[dest]);
}

std::shared_ptr<Sequence>
Sequencer::copySequence(const std::shared_ptr<Sequence> &source,
                        const SequenceIndex destinationIndex)
{
    auto copy = sequences[destinationIndex];

    if (!copy)
    {
        copy = makeNewSequence(destinationIndex);
    }

    copy->init(source->getLastBarIndex());
    copySequenceParameters(source, copy);

    for (int i = 0; i < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; i++)
    {
        copyTrack(source->getTrack(i), copy->getTrack(i));
    }

    return copy;
}

void Sequencer::copySequenceParameters(const std::shared_ptr<Sequence> &source,
                                       const std::shared_ptr<Sequence> &dest)
{
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
    copyTempoChangeEvents(source, dest);
}

void Sequencer::copyTempoChangeEvents(const std::shared_ptr<Sequence> &src,
                                      const std::shared_ptr<Sequence> &dst)
{
    for (const auto &e1 : src->getTempoChangeEvents())
    {
        dst->addTempoChangeEvent(e1->getTick(), e1->getRatio());
    }
}

void Sequencer::copyTrack(const int sourceTrackIndex,
                          const int destinationTrackIndex,
                          const int sourceSequenceIndex,
                          const int destinationSequenceIndex) const
{
    if (sourceSequenceIndex == destinationSequenceIndex &&
        sourceTrackIndex == destinationTrackIndex)
    {
        return;
    }

    const auto src = sequences[sourceSequenceIndex]->getTrack(sourceTrackIndex);
    const auto dest =
        sequences[destinationSequenceIndex]->purgeTrack(destinationTrackIndex);
    copyTrack(src, dest);
}

void Sequencer::copySong(const int source, const int dest)
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

    for (int i = 0; i < s0->getStepCount(); i++)
    {
        s1->insertStep(i);
        const auto step = s1->getStep(i).lock();
        step->setRepeats(s0->getStep(i).lock()->getRepeats());
        step->setSequence(s0->getStep(i).lock()->getSequence());
    }

    s1->setFirstStep(s0->getFirstStep());
    s1->setLastStep(s0->getLastStep());
    s1->setName(s0->getName());
    s1->setLoopEnabled(s0->isLoopEnabled());
}

void Sequencer::copyTrack(const std::shared_ptr<Track> &src,
                          const std::shared_ptr<Track> &dest)
{
    dest->setEventStates(src->getEventStates());
    copyTrackParameters(src, dest);
}

void Sequencer::copyTrackParameters(const std::shared_ptr<Track> &source,
                                    const std::shared_ptr<Track> &dest)
{
    dest->setUsed(source->isUsed());
    dest->setOn(source->isOn());
    dest->setDeviceIndex(source->getDeviceIndex());
    dest->setBusType(source->getBusType());
    dest->setVelocityRatio(source->getVelocityRatio());
    dest->setProgramChange(source->getProgramChange());
    dest->setName(source->getName());
}

std::string Sequencer::getDefaultTrackName(const int i)
{
    return defaultTrackNames[i];
}

std::vector<std::string> &Sequencer::getDefaultTrackNames()
{
    return defaultTrackNames;
}

void Sequencer::setDefaultTrackName(const std::string &s, const int i)
{
    defaultTrackNames[i] = s;
}

std::shared_ptr<Sequence> Sequencer::getSelectedSequence()
{
    if (const bool songMode = isSongModeEnabled();
        songMode &&
        songs[getScreens()->get<ScreenId::SongScreen>()->getSelectedSongIndex()]
                ->getStepCount() != 0)
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
        const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
        const auto song = songs[songScreen->getSelectedSongIndex()];

        if (!song->isUsed())
        {
            return NoSequenceIndex;
        }

        int step = songScreen->getOffset() + 1;

        if (step >= song->getStepCount())
        {
            step = song->getStepCount() - 1;
        }

        const auto songSeqIndex = song->getStep(step).lock()->getSequence();
        return songSeqIndex;
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

bool Sequencer::isSongModeEnabled() const
{
    return screengroups::isSongScreen(layeredScreen->getCurrentScreenId());
}

SequenceIndex Sequencer::getSongSequenceIndex() const
{
    const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    const auto song = songs[songScreen->getSelectedSongIndex()];
    auto step = songScreen->getOffset() + 1;

    if (step >= song->getStepCount())
    {
        step = song->getStepCount() - 1;
    }

    if (step < 0)
    {
        return NoSequenceIndex;
    }

    return song->getStep(step).lock()->getSequence();
}

bool Sequencer::isSecondSequenceEnabled() const
{
    return secondSequenceEnabled.load();
}

void Sequencer::setSecondSequenceEnabled(const bool b)
{
    secondSequenceEnabled.store(b);
}

void Sequencer::flushTrackNoteCache()
{
    for (const auto &t : getCurrentlyPlayingSequence()->getTracks())
    {
        t->flushNoteCache();
    }
}

void Sequencer::storeSelectedSequenceInUndoPlaceHolder()
{
    auto copy =
        copySequence(sequences[getSelectedSequenceIndex()], UndoSequenceIndex);

    undoSeqAvailable = true;

    concurrency::Task uiTask;
    uiTask.set(
        [this]
        {
            hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
                ->setEnabled(true);
        });
    layeredScreen->postToUiThread(uiTask);
}

void Sequencer::resetUndo()
{
    sequences[UndoSequenceIndex]->setUsed(false);
    undoSeqAvailable = false;
    hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)->setEnabled(false);
}

std::shared_ptr<Sequence> Sequencer::createSeqInPlaceHolder()
{
    makeNewSequence(PlaceholderSequenceIndex);
    return sequences[PlaceholderSequenceIndex];
}

void Sequencer::clearPlaceHolder() const
{
    sequences[PlaceholderSequenceIndex]->setUsed(false);
}

void Sequencer::movePlaceHolderTo(const int destIndex)
{
    sequences[destIndex].swap(sequences[PlaceholderSequenceIndex]);
    clearPlaceHolder();
}

std::shared_ptr<Sequence> Sequencer::getPlaceHolder()
{
    return sequences[PlaceholderSequenceIndex];
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

std::shared_ptr<DrumBus>
Sequencer::getDrumBus(const DrumBusIndex drumBusIndex) const
{
    assert(drumBusIndex.get() >= 0 &&
           drumBusIndex.get() <= Mpc2000XlSpecs::DRUM_BUS_COUNT - 1);
    auto result = getBus<DrumBus>(drumBusIndexToDrumBusType(drumBusIndex));
    assert(result);
    return result;
}

std::shared_ptr<DrumBus> Sequencer::getDrumBus(const BusType busType) const
{
    assert(isDrumBusType(busType));
    auto result = getBus<DrumBus>(busType);
    assert(result);
    return result;
}

template std::shared_ptr<MidiBus> Sequencer::getBus(BusType) const;
template std::shared_ptr<DrumBus> Sequencer::getBus(BusType) const;
