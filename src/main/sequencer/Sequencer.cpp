#include "Sequencer.hpp"

#include "MpcSpecs.hpp"

#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/SequenceStateManager.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"

#include "sampler/Sampler.hpp"

#include "engine/Voice.hpp"
#include "engine/SequencerPlaybackEngine.hpp"

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

#include <chrono>

using namespace mpc;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::performance;
using namespace mpc::sampler;
using namespace mpc::audiomidi;

uint32_t Sequencer::quarterNotesToTicks(const double quarterNotes)
{
    return static_cast<uint32_t>(
        std::floor(quarterNotes * static_cast<double>(TICKS_PER_QUARTER_NOTE)));
}

double Sequencer::ticksToQuarterNotes(const uint32_t ticks)
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
    const std::function<bool()> &isSixteenLevelsEnabled,
    const std::function<std::shared_ptr<SequencerPlaybackEngine>()>
        &getSequencerPlaybackEngine)
    : getScreens(getScreens), isBouncePrepared(isBouncePrepared),
      startBouncing(startBouncing), hardware(hardware), isBouncing(isBouncing),
      stopBouncing(stopBouncing), layeredScreen(layeredScreen), voices(voices),
      isAudioServerRunning(isAudioServerRunning),
      isEraseButtonPressed(isEraseButtonPressed),
      performanceManager(performanceManager), sampler(sampler),
      eventHandler(eventHandler),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled),
      getSequencerPlaybackEngine(getSequencerPlaybackEngine)
{
    stateManager = std::make_shared<SequencerStateManager>(this);
    sequenceStateManager = std::make_shared<SequenceStateManager>();
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
    transport = std::make_shared<Transport>(*this, getSequencerPlaybackEngine);

    for (int midiBusIndex = 0; midiBusIndex < Mpc2000XlSpecs::MIDI_BUS_COUNT;
         ++midiBusIndex)
    {
        buses.emplace_back(std::make_shared<MidiBus>());
    }

    for (int drumBusIndex = 0; drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT;
         ++drumBusIndex)
    {
        buses.emplace_back(std::make_shared<DrumBus>(DrumBusIndex(drumBusIndex),
                                                     performanceManager));
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

std::shared_ptr<sequencer::Track> Sequencer::getSelectedTrack()
{
    if (!getSelectedSequence())
    {
        return {};
    }

    return getSelectedSequence()->getTrack(selectedTrackIndex);
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

std::shared_ptr<sequencer::Sequence> Sequencer::getSequence(const int i)
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

void Sequencer::trackUp()
{
    if (selectedTrackIndex == 63)
    {
        return;
    }
    selectedTrackIndex++;
}

void Sequencer::trackDown()
{
    if (selectedTrackIndex == 0)
    {
        return;
    }
    selectedTrackIndex--;
}

void Sequencer::undoSeq()
{
    if (transport->isPlaying())
    {
        return;
    }

    if (!undoPlaceHolder)
    {
        return;
    }

    auto s = copySequence(undoPlaceHolder);
    const auto selectedSequenceIndex = getSelectedSequenceIndex();
    auto copy = copySequence(sequences[selectedSequenceIndex]);
    undoPlaceHolder.swap(copy);

    sequences[selectedSequenceIndex].swap(s);

    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();

    sequences[selectedSequenceIndex]->syncTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));

    undoSeqAvailable = !undoSeqAvailable;

    hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
        ->setEnabled(undoSeqAvailable);
}

void Sequencer::setSequence(const SequenceIndex sequenceIndex,
                            std::shared_ptr<Sequence> s)
{
    sequences[sequenceIndex].swap(s);
    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    sequences[sequenceIndex]->syncTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));
}

void Sequencer::purgeAllSequences()
{
    for (int i = 0; i < Mpc2000XlSpecs::SEQUENCE_COUNT; i++)
    {
        purgeSequence(i);
    }

    setSelectedSequenceIndex(MinSequenceIndex, true);
}

std::shared_ptr<sequencer::Sequence> Sequencer::makeNewSequence()
{
    return std::make_shared<Sequence>(
        performanceManager,
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
            return performanceManager->getSnapshot().isProgramPadPressed(
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
    sequences[i] = makeNewSequence();
    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    sequences[i]->syncTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));
    std::string res = defaultSequenceName;
    res.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
    sequences[i]->setName(res);
}

void Sequencer::copySequence(const int source, const int destination)
{
    auto copy = copySequence(sequences[source]);
    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    sequences[destination].swap(copy);
    sequences[destination]->syncTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));
    sequences[destination]->initLoop();
}

void Sequencer::copySequenceParameters(const int source, const int dest) const
{
    copySequenceParameters(sequences[source], sequences[dest]);
}

std::shared_ptr<sequencer::Sequence>
Sequencer::copySequence(const std::shared_ptr<Sequence> &source)
{
    auto copy = makeNewSequence();
    copy->init(source->getLastBarIndex());
    copySequenceParameters(source, copy);

    for (int i = 0; i < 64; i++)
    {
        copyTrack(source->getTrack(i), copy->getTrack(i));
    }

    copy->getTempoChangeTrack()->removeEvents();

    for (auto &event : source->getTempoChangeTrack()->getEvents())
    {
        copy->getTempoChangeTrack()->cloneEventIntoTrack(event,
                                                         event->getTick());
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
    dest->setBarLengths(source->getBarLengthsInTicks());
    dest->setNumeratorsAndDenominators(source->getNumerators(),
                                       source->getDenominators());
    dest->setLoopStart(source->getLoopStart());
    dest->setLoopEnd(source->getLoopEnd());
    copyTempoChangeEvents(source, dest);
}

void Sequencer::copyTempoChangeEvents(const std::shared_ptr<Sequence> &src,
                                      const std::shared_ptr<Sequence> &dst)
{
    for (const auto &e1 : src->getTempoChangeEvents())
    {
        const auto copy = dst->addTempoChangeEvent(e1->getTick());
        copy->setRatio(e1->getRatio());
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
    if (src == dest)
    {
        return;
    }

    dest->setTrackIndex(src->getIndex());

    for (auto &e : src->getEvents())
    {
        dest->cloneEventIntoTrack(e, e->getTick());
    }

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

std::shared_ptr<sequencer::Sequence> Sequencer::getSelectedSequence()
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

std::vector<std::shared_ptr<sequencer::Sequence>> Sequencer::getUsedSequences() const
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

std::shared_ptr<sequencer::Sequence> Sequencer::getCurrentlyPlayingSequence()
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
    selectedTrackIndex = i;
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

        const auto seqIndexShouldBeDerivedFromSongStep =
            songScreen->getOffset() + 1 < song->getStepCount();

        const auto songSeqIndex =
            seqIndexShouldBeDerivedFromSongStep
                ? song->getStep(songScreen->getOffset() + 1)
                      .lock()
                      ->getSequence()
                : NoSequenceIndex;
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

    if (step > song->getStepCount() - 1)
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
    auto copy = copySequence(sequences[getSelectedSequenceIndex()]);
    undoPlaceHolder.swap(copy);

    undoSeqAvailable = true;

    layeredScreen->postToUiThread(
        [this]
        {
            hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)
                ->setEnabled(true);
        });
}

void Sequencer::resetUndo()
{
    undoPlaceHolder.reset();
    undoSeqAvailable = false;
    hardware->getLed(hardware::ComponentId::UNDO_SEQ_LED)->setEnabled(false);
}

std::shared_ptr<sequencer::Sequence> Sequencer::createSeqInPlaceHolder()
{
    placeHolder = makeNewSequence();
    return placeHolder;
}

void Sequencer::clearPlaceHolder()
{
    placeHolder.reset();
}

void Sequencer::movePlaceHolderTo(const int destIndex)
{
    sequences[destIndex].swap(placeHolder);
    sequences[destIndex]->syncTrackEventIndices(quarterNotesToTicks(
        stateManager->getSnapshot().getPositionQuarterNotes()));
    clearPlaceHolder();
}

std::shared_ptr<sequencer::Sequence> Sequencer::getPlaceHolder()
{
    return placeHolder;
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
