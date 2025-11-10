#include "Sequencer.hpp"

#include "sequencer/SequencerStateManager.hpp"

#include "MpcSpecs.hpp"

#include "controller/ClientEventController.hpp"
#include "engine/Voice.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"

#include "StrUtil.hpp"

#include <chrono>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::eventregistry;
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

Sequencer::Sequencer(std::shared_ptr<LayeredScreen> layeredScreen,
                     std::function<std::shared_ptr<Screens>()> getScreens,
                     std::vector<std::shared_ptr<Voice>> *voices,
                     const std::function<bool()> &isAudioServerRunning,
                     const std::shared_ptr<hardware::Hardware> &hardware,
                     const std::function<bool()> &isBouncePrepared,
                     const std::function<void()> &startBouncing,
                     const std::function<void()> &stopBouncing,
                     std::function<bool()> isBouncing,
                     const std::function<bool()> &isEraseButtonPressed,
                     std::shared_ptr<EventRegistry> eventRegistry,
                     std::shared_ptr<Sampler> sampler,
                     const std::shared_ptr<EventHandler> &eventHandler,
                     std::function<bool()> isSixteenLevelsEnabled,
                     std::shared_ptr<Clock> clock,
                     std::function<int()> getSampleRate,
                     std::function<bool()> isRecMainWithoutPlaying,
                     std::function<bool()> isNoteRepeatLockedOrPressed,
                     std::function<std::shared_ptr<AudioMixer>()> getAudioMixer,
                     std::function<bool()> isFullLevelEnabled,
                     std::function<std::vector<MixerInterconnection *> &()>
                         getMixerInterconnections)
    : layeredScreen(layeredScreen), getScreens(getScreens), voices(voices),
      isAudioServerRunning(isAudioServerRunning), hardware(hardware),
      isBouncePrepared(isBouncePrepared), startBouncing(startBouncing),
      stopBouncing(stopBouncing), isBouncing(isBouncing),
      isEraseButtonPressed(isEraseButtonPressed), eventRegistry(eventRegistry),
      sampler(sampler), eventHandler(eventHandler),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled)
{
    stateManager = std::make_shared<SequencerStateManager>();

    frameSequencer = std::make_shared<FrameSeq>(
        eventRegistry, this, clock, layeredScreen, isBouncing, getSampleRate,
        isRecMainWithoutPlaying,
        [sampler](const int velo, const int frameOffset)
        {
            sampler->playMetronome(velo, frameOffset);
        },
        getScreens, isNoteRepeatLockedOrPressed, sampler, getAudioMixer,
        isFullLevelEnabled, isSixteenLevelsEnabled, hardware->getSlider(),
        voices, getMixerInterconnections);
}

std::shared_ptr<SequencerStateManager> Sequencer::getStateManager()
{
    return stateManager;
}

void Sequencer::init()
{
    for (int midiBusIndex = 0; midiBusIndex < Mpc2000XlSpecs::MIDI_BUS_COUNT;
         ++midiBusIndex)
    {
        buses.emplace_back(std::make_shared<MidiBus>());
    }

    for (int drumBusIndex = 0; drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT;
         ++drumBusIndex)
    {
        buses.emplace_back(std::make_shared<DrumBus>(drumBusIndex));
    }

    lastTap = currentTimeMillis();
    nextSq = -1;

    auto userScreen = getScreens()->get<ScreenId::UserScreen>();
    defaultSequenceName = StrUtil::trim(userScreen->sequenceName);

    for (int i = 0; i < 64; i++)
    {
        std::string name = "Track-";
        name = name.append(StrUtil::padLeft(std::to_string(i + 1), "0", 2));
        defaultTrackNames.push_back(name);
    }

    activeTrackIndex = 0;

    recordingModeMulti = userScreen->recordingModeMulti;

    soloEnabled = false;
    tempoSourceSequenceEnabled = true;
    countEnabled = true;
    recording = false;

    tempo = userScreen->tempo;

    metronomeOnly = false;
    activeSequenceIndex = 0;
    currentlyPlayingSequenceIndex = 0;
    stateManager->enqueue(SetSongModeEnabled{false});

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

int Sequencer::getActiveSequenceIndex() const
{
    return activeSequenceIndex;
}

std::shared_ptr<Track> Sequencer::getActiveTrack()
{
    if (!getActiveSequence())
    {
        return {};
    }

    return getActiveSequence()->getTrack(activeTrackIndex);
}

void Sequencer::playToTick(const int targetTick) const
{
    auto seqIndex = stateManager->getSnapshot().isSongModeEnabled()
                        ? getSongSequenceIndex()
                        : currentlyPlayingSequenceIndex;
    auto seq = sequences[seqIndex].get();
    auto secondSequenceScreen = getScreens()->get<ScreenId::SecondSeqScreen>();

    for (int i = 0; i < 2; i++)
    {
        if (i == 1)
        {
            if (!secondSequenceEnabled || metronomeOnly ||
                secondSequenceScreen->sq ==
                    seqIndex) // Real 2KXL would play all events twice for the
                              // last clause
            {
                break;
            }

            seq = sequences[secondSequenceScreen->sq].get();

            if (!seq->isUsed())
            {
                break;
            }
        }

        if (!metronomeOnly)
        {
            for (auto &track : seq->getTracks())
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

void Sequencer::setTempo(double newTempo)
{
    if (newTempo < 30.0)
    {
        newTempo = 30.0;
    }
    else if (newTempo > 300.0)
    {
        newTempo = 300.0;
    }

    auto s = getActiveSequence();
    auto tce = getCurrentTempoChangeEvent();

    if (!s || !s->isUsed() || !tempoSourceSequenceEnabled)
    {
        if (tce)
        {
            auto candidate = newTempo / (tce->getRatio() * 0.001);

            if (candidate < 30.0)
            {
                candidate = 30.0;
            }
            else if (candidate > 300.0)
            {
                candidate = 300.0;
            }

            tempo = candidate;
        }
        else
        {
            tempo = newTempo;
        }
        return;
    }

    if (tce && tce->getTick() == 0 && s->isTempoChangeOn())
    {
        s->setInitialTempo(newTempo / (tce->getRatio() * 0.001));
    }
    else if (s->isTempoChangeOn())
    {
        auto initialTempo = s->getInitialTempo();
        auto ratio = newTempo / initialTempo;
        tce->setRatio(static_cast<int>(round(ratio * 1000.0)));
    }
    else
    {
        s->setInitialTempo(newTempo);
    }
}

double Sequencer::getTempo()
{
    if (!isPlaying() && !getActiveSequence()->isUsed())
    {
        return tempo;
    }

    auto seq = getActiveSequence();

    if (layeredScreen->getCurrentScreenName() == "song")
    {
        if (!seq->isUsed())
        {
            return 120.0;
        }
    }

    auto tce = getCurrentTempoChangeEvent();

    if (tempoSourceSequenceEnabled)
    {
        auto ignoreTempoChangeScreen =
            getScreens()->get<ScreenId::IgnoreTempoChangeScreen>();

        if (seq->isTempoChangeOn() ||
            (stateManager->getSnapshot().isSongModeEnabled() &&
             !ignoreTempoChangeScreen->getIgnore()))
        {
            if (tce)
            {
                return tce->getTempo();
            }
        }

        return getActiveSequence()->getInitialTempo();
    }

    if (seq->isTempoChangeOn() && tce)
    {
        return tempo * tce->getRatio() * 0.001;
    }

    return tempo;
}

std::shared_ptr<TempoChangeEvent> Sequencer::getCurrentTempoChangeEvent()
{
    auto index = -1;
    auto s = getActiveSequence();

    if (!s->isUsed())
    {
        return {};
    }

    for (auto &tce : s->getTempoChangeEvents())
    {
        if (getTickPosition() >= tce->getTick())
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

bool Sequencer::isTempoSourceSequenceEnabled() const
{
    return tempoSourceSequenceEnabled;
}

void Sequencer::setTempoSourceSequence(const bool b)
{
    tempoSourceSequenceEnabled = b;
}

bool Sequencer::isRecordingOrOverdubbing() const
{
    return recording || overdubbing;
}

bool Sequencer::isRecording() const
{
    return recording;
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
                for (auto &voice : (*voices))
                {
                    if (!voice->isFinished() && voice->getNote() == note &&
                        voice->getVoiceOverlapMode() ==
                            sampler::VoiceOverlapMode::NOTE_OFF &&
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

void Sequencer::setActiveSequenceIndex(int i)
{
    i = std::clamp(i, 0, 98);

    activeSequenceIndex = i;

    if (!isPlaying())
    {
        stateManager->enqueue(SetPositionQuarterNotes{0.0});
    }
}

bool Sequencer::isCountEnabled() const
{
    return countEnabled;
}

void Sequencer::setCountEnabled(const bool b)
{
    countEnabled = b;
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

int Sequencer::getActiveTrackIndex() const
{
    return activeTrackIndex;
}

void Sequencer::trackUp()
{
    if (activeTrackIndex == 63)
    {
        return;
    }
    activeTrackIndex++;
}

void Sequencer::trackDown()
{
    if (activeTrackIndex == 0)
    {
        return;
    }
    activeTrackIndex--;
}

bool Sequencer::isPlaying() const
{
    if (!isAudioServerRunning())
    {
        return false;
    }

    return !metronomeOnly && frameSequencer->isRunning();
}

void Sequencer::play(const bool fromStart)
{
    if (isPlaying())
    {
        return;
    }

    endOfSong = false;
    auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    auto currentSong = songs[songScreen->getActiveSongIndex()];

    const auto snapshot = stateManager->getSnapshot();
    const bool songMode = snapshot.isSongModeEnabled();

    if (songMode)
    {
        if (!currentSong->isUsed())
        {
            return;
        }

        if (fromStart)
        {
            songScreen->setOffset(-1);
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
            !sequences[currentStep->getSequence()]->isUsed())
        {
            return;
        }
    }

    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    move(positionQuarterNotes);

    currentlyPlayingSequenceIndex = activeSequenceIndex;

    auto countMetronomeScreen =
        getScreens()->get<ScreenId::CountMetronomeScreen>();
    auto countInMode = countMetronomeScreen->getCountInMode();

    if (!countEnabled || countInMode == 0 ||
        (countInMode == 1 && !isRecordingOrOverdubbing()))
    {
        if (fromStart)
        {
            move(0);
        }
    }

    auto s = getActiveSequence();

    if (countEnabled && !songMode)
    {
        if (countInMode == 2 ||
            (countInMode == 1 && (isRecordingOrOverdubbing())))
        {
            if (fromStart)
            {
                move(ticksToQuarterNotes(s->getLoopStart()));
            }
            else
            {
                move(ticksToQuarterNotes(
                    s->getFirstTickOfBar(getCurrentBarIndex())));
            }

            countInStartPos = quarterNotesToTicks(positionQuarterNotes);
            countInEndPos = s->getLastTickOfBar(getCurrentBarIndex());

            countingIn = true;
        }
    }

    if (!songMode)
    {
        if (!s->isUsed())
        {
            recording = false;
            overdubbing = false;
            return;
        }

        s->initLoop();

        if (recording || overdubbing)
        {
            auto copy = copySequence(s);
            undoPlaceHolder.swap(copy);
            undoSeqAvailable = true;
            hardware->getLed(mpc::hardware::ComponentId::UNDO_SEQ_LED)
                ->setEnabled(true);
        }
    }

    if (isBouncePrepared())
    {
        startBouncing();
    }
    else
    {
        frameSequencer->start();
    }
}

void Sequencer::undoSeq()
{
    if (isPlaying())
    {
        return;
    }

    if (!undoPlaceHolder)
    {
        return;
    }

    auto s = copySequence(undoPlaceHolder);
    auto copy = copySequence(sequences[activeSequenceIndex]);
    undoPlaceHolder.swap(copy);

    sequences[activeSequenceIndex].swap(s);

    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();

    sequences[activeSequenceIndex]->resetTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));

    undoSeqAvailable = !undoSeqAvailable;

    hardware->getLed(mpc::hardware::ComponentId::UNDO_SEQ_LED)
        ->setEnabled(undoSeqAvailable);
}

void Sequencer::playFromStart()
{
    if (isPlaying())
    {
        return;
    }

    play(true);
}

void Sequencer::play()
{
    if (isPlaying())
    {
        return;
    }

    play(false);
}

void Sequencer::rec()
{
    if (isPlaying())
    {
        return;
    }

    recording = true;

    play(false);
}

void Sequencer::recFromStart()
{
    if (isPlaying())
    {
        return;
    }

    recording = true;
    play(true);
}

void Sequencer::overdub()
{
    if (isPlaying())
    {
        return;
    }

    overdubbing = true;
    play(false);
}

void Sequencer::switchRecordToOverdub()
{
    if (!recording)
    {
        return;
    }

    recording = false;
    overdubbing = true;
    hardware->getLed(hardware::ComponentId::REC_LED)->setEnabled(false);
    hardware->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(true);
}

void Sequencer::overdubFromStart()
{
    if (isPlaying())
    {
        return;
    }

    overdubbing = true;
    play(true);
}

void Sequencer::stop()
{
    stop(StopMode::AT_START_OF_BUFFER);
}

void Sequencer::stop(const StopMode stopMode)
{
    bool bouncing = isBouncing();

    if (!isPlaying() && !bouncing)
    {
        const auto snapshot = stateManager->getSnapshot();
        const double positionQuarterNotes = snapshot.getPositionQuarterNotes();

        if (positionQuarterNotes != 0.0)
        {
            setBar(0); // real 2kxl doesn't do this
        }

        return;
    }

    playedStepRepetitions = 0;
    stateManager->enqueue(SetSongModeEnabled{false});
    nextSq = -1;

    lastNotifiedBar = -1;
    lastNotifiedBeat = -1;
    lastNotifiedClock = -1;

    auto s1 = getActiveSequence();
    auto s2 = getCurrentlyPlayingSequence();
    auto pos = getTickPosition();

    if (pos > s1->getLastTick())
    {
        pos = s1->getLastTick();
    }

    // const int frameOffset = stopMode == AT_START_OF_BUFFER
    //                             ? 0
    //                             : frameSequencer->getEventFrameOffset();

    frameSequencer->stop();

    recording = false;
    overdubbing = false;

    if (countingIn)
    {
        pos = countInStartPos;
        countInStartPos = -1;
        countInEndPos = -1;
        countingIn = false;
    }

    move(ticksToQuarterNotes(pos));

    if (!bouncing)
    {
        // mpc.getSampler()->stopAllVoices(frameOffset);
    }

    for (int i = 0; i < 16; i++)
    {
        auto pad = hardware->getPad(i);
        pad->release();
    }

    auto songScreen = getScreens()->get<ScreenId::SongScreen>();

    if (endOfSong)
    {
        songScreen->setOffset(songScreen->getOffset() + 1);
    }

    auto vmpcDirectToDiskRecorderScreen =
        getScreens()->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (bouncing && vmpcDirectToDiskRecorderScreen->getRecord() != 4)
    {
        stopBouncing();
    }

    hardware->getLed(hardware::ComponentId::PLAY_LED)->setEnabled(false);
    hardware->getLed(hardware::ComponentId::REC_LED)->setEnabled(false);
    hardware->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(false);
}

bool Sequencer::isCountingIn() const
{
    return countingIn;
}

void Sequencer::setCountingIn(const bool b)
{
    countingIn = b;

    if (!countingIn)
    {
        countInStartPos = -1;
        countInEndPos = -1;
    }
}

void Sequencer::setSequence(const int i, std::shared_ptr<Sequence> s)
{
    sequences[i].swap(s);
    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    sequences[i]->resetTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));
}

void Sequencer::purgeAllSequences()
{
    for (int i = 0; i < 99; i++)
    {
        purgeSequence(i);
    }

    activeSequenceIndex = 0;
}

std::shared_ptr<Sequence> Sequencer::makeNewSequence()
{
    return std::make_shared<Sequence>(
        [&](const int trackIndex)
        {
            return defaultTrackNames[trackIndex];
        },
        [&]
        {
            return getTickPosition();
        },
        getScreens,
        [&]
        {
            return isRecordingModeMulti();
        },
        [&]
        {
            return getActiveSequence();
        },
        [&]
        {
            return getAutoPunchMode();
        },
        [&](const int busIndex)
        {
            return getBus<Bus>(busIndex);
        },
        [&]
        {
            return isEraseButtonPressed();
        },
        [&](const int programPadIndex,
            const std::shared_ptr<sampler::Program> &program)
        {
            return eventRegistry->getSnapshot().isProgramPadPressed(
                programPadIndex, program);
        },
        sampler, eventHandler,
        [&]
        {
            return isSixteenLevelsEnabled();
        },
        [&]
        {
            return getActiveTrackIndex();
        },
        [&]
        {
            return isRecording();
        },
        [&]
        {
            return isOverdubbing();
        },
        [&]
        {
            return isPunchEnabled();
        },
        [&]
        {
            return getPunchInTime();
        },
        [&]
        {
            return getPunchOutTime();
        },
        [&]
        {
            return isSoloEnabled();
        },
        [&]
        {
            return getCurrentBarIndex();
        });
}

void Sequencer::purgeSequence(const int i)
{
    sequences[i] = makeNewSequence();
    const auto snapshot = stateManager->getSnapshot();
    const double positionQuarterNotes = snapshot.getPositionQuarterNotes();
    sequences[i]->resetTrackEventIndices(
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
    sequences[destination]->resetTrackEventIndices(
        quarterNotesToTicks(positionQuarterNotes));
    sequences[destination]->initLoop();
}

void Sequencer::copySequenceParameters(const int source, const int dest) const
{
    copySequenceParameters(sequences[source], sequences[dest]);
}

std::shared_ptr<Sequence>
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

void Sequencer::copySequenceParameters(
    const std::shared_ptr<Sequence> &source,
    const std::shared_ptr<Sequence> &dest) const
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

void Sequencer::copyTempoChangeEvents(
    const std::shared_ptr<Sequence> &src,
    const std::shared_ptr<Sequence> &dst) const
{
    for (auto &e1 : src->getTempoChangeEvents())
    {
        auto copy = dst->addTempoChangeEvent(e1->getTick());
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

    auto src = sequences[sourceSequenceIndex]->getTrack(sourceTrackIndex);
    auto dest =
        sequences[destinationSequenceIndex]->purgeTrack(destinationTrackIndex);
    copyTrack(src, dest);
}

void Sequencer::copySong(const int source, const int dest)
{
    if (source == dest)
    {
        return;
    }

    auto s0 = songs[source];

    if (!s0->isUsed())
    {
        return;
    }

    deleteSong(dest);

    auto s1 = songs[dest];

    s1->setUsed(true);

    for (int i = 0; i < s0->getStepCount(); i++)
    {
        s1->insertStep(i);
        auto step = s1->getStep(i).lock();
        step->setRepeats(s0->getStep(i).lock()->getRepeats());
        step->setSequence(s0->getStep(i).lock()->getSequence());
    }

    s1->setFirstStep(s0->getFirstStep());
    s1->setLastStep(s0->getLastStep());
    s1->setName(s0->getName());
    s1->setLoopEnabled(s0->isLoopEnabled());
}

void Sequencer::copyTrack(const std::shared_ptr<Track> &src,
                          const std::shared_ptr<Track> &dest) const
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
    dest->setBusNumber(source->getBus());
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

int Sequencer::getCurrentBarIndex()
{
    auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();
    auto pos = isCountingIn()
                   ? quarterNotesToTicks(getPlayStartPositionQuarterNotes())
                   : getTickPosition();

    if (pos == s->getLastTick())
    {
        return s->getLastBarIndex() + 1;
    }

    auto &barLengths = s->getBarLengthsInTicks();

    int tickCounter = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i > s->getLastBarIndex())
        {
            return 0; // Should not happen
        }

        tickCounter += barLengths[i];

        if (tickCounter > pos)
        {
            return i;
        }
    }

    return 0;
}

int Sequencer::getCurrentBeatIndex()
{
    auto s = isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();
    auto pos = isCountingIn()
                   ? quarterNotesToTicks(getPlayStartPositionQuarterNotes())
                   : getTickPosition();

    if (pos == s->getLastTick())
    {
        return 0;
    }

    auto index = pos;

    if (isPlaying() && !countingIn)
    {
        index = getTickPosition();

        if (index > s->getLastTick())
        {
            index %= s->getLastTick();
        }
    }

    auto ts = s->getTimeSignature();
    auto den = ts.getDenominator();
    auto denTicks = 96 * (4.0 / den);

    if (index == 0)
    {
        return 0;
    }

    int barStartPos = 0;
    auto barCounter = 0;

    const auto currentBarIndex = getCurrentBarIndex();

    for (auto &l : s->getBarLengthsInTicks())
    {
        if (barCounter == currentBarIndex)
        {
            break;
        }

        barStartPos += l;
        barCounter++;
    }

    const auto beatIndex =
        static_cast<int>(floor((index - barStartPos) / denTicks));
    return beatIndex;
}

int Sequencer::getCurrentClockNumber()
{
    auto sequence =
        isPlaying() ? getCurrentlyPlayingSequence() : getActiveSequence();

    auto clock = isCountingIn()
                     ? quarterNotesToTicks(getPlayStartPositionQuarterNotes())
                     : getTickPosition();

    if (clock == sequence->getLastTick())
    {
        return 0;
    }

    if (isPlaying() && !countingIn)
    {
        if (clock > sequence->getLastTick())
        {
            clock %= sequence->getLastTick();
        }
    }

    auto ts = sequence->getTimeSignature();
    auto den = ts.getDenominator();
    auto denTicks = 96 * (4.0 / den);

    if (clock == 0)
    {
        return 0;
    }

    auto barCounter = 0;
    auto currentBarIndex = getCurrentBarIndex();

    for (auto &l : sequence->getBarLengthsInTicks())
    {
        if (barCounter == currentBarIndex)
        {
            break;
        }

        clock -= l;
        barCounter++;
    }

    auto currentBeatIndex = getCurrentBeatIndex();

    for (int i = 0; i < currentBeatIndex; i++)
    {
        clock -= denTicks;
    }

    return clock;
}

void Sequencer::setBar(int i)
{
    if (isPlaying())
    {
        return;
    }

    if (i < 0)
    {
        move(0);
        return;
    }

    auto s = getActiveSequence();

    if (i > s->getLastBarIndex() + 1)
    {
        i = s->getLastBarIndex() + 1;
    }

    if (s->getLastBarIndex() == 998 && i > 998)
    {
        i = 998;
    }

    auto ts = s->getTimeSignature();
    auto den = ts.getDenominator();
    auto denTicks = static_cast<int>(96 * (4.0 / den));

    if (i != s->getLastBarIndex() + 1)
    {
        ts.setNumerator(s->getNumerator(i));
        ts.setDenominator(s->getDenominator(i));
    }

    auto &barLengths = s->getBarLengthsInTicks();
    auto currentClock = getCurrentClockNumber();
    auto currentBeat = getCurrentBeatIndex();
    int pos = 0;
    auto barCounter = 0;

    for (auto &l : barLengths)
    {
        if (barCounter == i)
        {
            break;
        }

        pos += l;
        barCounter++;
    }

    pos += currentBeat * denTicks;
    pos += currentClock;

    if (pos > s->getLastTick())
    {
        pos = s->getLastTick();
    }

    move(ticksToQuarterNotes(pos));

    setBeat(0);
    setClock(0);
}

void Sequencer::setBeat(int i)
{
    if (isPlaying())
    {
        return;
    }

    if (i < 0)
    {
        i = 0;
    }

    auto s = getActiveSequence();
    auto pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    auto ts = s->getTimeSignature();
    auto num = ts.getNumerator();

    if (i >= num)
    {
        i = num - 1;
    }

    const auto difference = i - getCurrentBeatIndex();

    const auto denTicks = 96 * (4.0 / ts.getDenominator());
    pos += difference * denTicks;
    move(ticksToQuarterNotes(pos));
}

void Sequencer::setClock(int i)
{
    if (isPlaying())
    {
        return;
    }

    if (i < 0)
    {
        i = 0;
    }

    auto s = getActiveSequence();
    int pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    getCurrentClockNumber();
    auto den = s->getTimeSignature().getDenominator();
    auto denTicks = 96 * (4.0 / den);

    if (i > denTicks - 1)
    {
        i = denTicks - 1;
    }

    const int difference = i - getCurrentClockNumber();

    pos += difference;
    move(ticksToQuarterNotes(pos));
}

int Sequencer::getLoopEnd()
{
    return getActiveSequence()->getLoopEnd();
}

std::shared_ptr<Sequence> Sequencer::getActiveSequence()
{
    auto songScreen = getScreens()->get<ScreenId::SongScreen>();

    const auto snapshot = stateManager->getSnapshot();
    const bool songMode = snapshot.isSongModeEnabled();

    if (songMode &&
        songs[songScreen->getActiveSongIndex()]->getStepCount() != 0)
    {
        return sequences[getSongSequenceIndex() >= 0 ? getSongSequenceIndex()
                                                     : activeSequenceIndex];
    }

    return sequences[activeSequenceIndex];
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
        auto s = sequences[i];

        if (s->isUsed())
        {
            usedSeqs.push_back(i);
        }
    }

    return usedSeqs;
}

void Sequencer::goToPreviousEvent()
{
    auto s = getActiveSequence();
    auto t = s->getTrack(getActiveTrackIndex());

    auto newPos = 0;
    auto events = t->getEvents();
    std::reverse(events.begin(), events.end());

    for (auto &e : events)
    {
        if (e->getTick() < getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    move(ticksToQuarterNotes(newPos));
}

void Sequencer::goToNextEvent()
{
    auto s = getActiveSequence();
    auto t = s->getTrack(getActiveTrackIndex());

    auto newPos = s->getLastTick();

    for (auto &e : t->getEvents())
    {
        if (e->getTick() > getTickPosition())
        {
            newPos = e->getTick();
            break;
        }
    }

    move(ticksToQuarterNotes(newPos));
}

void Sequencer::goToPreviousStep()
{
    auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();

    const auto stepSize = timingCorrectScreen->getNoteValueLengthInTicks();
    const auto pos = getTickPosition();
    const auto stepCount =
        static_cast<int>(ceil(getActiveSequence()->getLastTick() / stepSize)) +
        1;
    std::vector<int> stepGrid(stepCount);

    for (int i = 0; i < stepGrid.size(); i++)
    {
        stepGrid[i] = i * stepSize;
    }

    auto prevStepIndex = 0;

    for (auto l : stepGrid)
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

    move(ticksToQuarterNotes(prevStepIndex * stepSize));
}

void Sequencer::goToNextStep()
{
    auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();

    const auto stepSize = timingCorrectScreen->getNoteValueLengthInTicks();
    const auto pos = getTickPosition();

    std::vector<int> stepGrid(
        ceil(getActiveSequence()->getLastTick() / stepSize));

    for (int i = 0; i < stepGrid.size(); i++)
    {
        stepGrid[i] = i * stepSize;
    }

    auto nextStepIndex = -1;

    for (auto l : stepGrid)
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

    move(ticksToQuarterNotes(nextStepIndex * stepSize));
}

void Sequencer::tap()
{
    if (isPlaying())
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
        auto l0 = taps[index];

        index--;

        if (index < 0)
        {
            index = 3;
        }

        auto l1 = taps[index];

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
    setTempo(newTempo);
}

void Sequencer::bumpPositionByTicks(const uint8_t tickCount)
{
    stateManager->enqueue(BumpPositionByTicks{tickCount});
}

void Sequencer::setPosition(const double positionQuarterNotesToUse)
{
    auto wrappedNewPosition = positionQuarterNotesToUse;

    const auto songSequenceIndex = getSongSequenceIndex();
    const bool songMode = stateManager->getSnapshot().isSongModeEnabled();

    if (songMode && songSequenceIndex == -1)
    {
        return;
    }

    const auto sequence = isPlaying() ? getCurrentlyPlayingSequence()
                                      : (songMode ? sequences[songSequenceIndex]
                                                  : getActiveSequence());

    const auto seqLengthQuarterNotes =
        ticksToQuarterNotes(sequence->getLastTick());

    if (wrappedNewPosition < 0 || wrappedNewPosition > seqLengthQuarterNotes)
    {
        wrappedNewPosition = fmod(wrappedNewPosition, seqLengthQuarterNotes);

        while (wrappedNewPosition < 0)
        {
            wrappedNewPosition += seqLengthQuarterNotes;
        }
    }

    stateManager->enqueue(SetPositionQuarterNotes{wrappedNewPosition});
}

void Sequencer::setPositionWithinSong(const double positionQuarterNotesToUse)
{
    if (layeredScreen->getCurrentScreenName() != "song")
    {
        return;
    }

    const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    const auto song = songs[songScreen->getActiveSongIndex()];

    uint32_t stepStartTick;
    uint32_t stepEndTick = 0;
    uint64_t songEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;
        const auto step = song->getStep(stepIndex);
        const auto sequence = getSequence(step.lock()->getSequence());

        if (sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          (sequence->getLastTick() * step.lock()->getRepeats());
        }

        songEndTick = stepEndTick;
    }

    const double songLengthQuarterNotes = ticksToQuarterNotes(songEndTick);

    auto wrappedNewPosition = positionQuarterNotesToUse;

    if (wrappedNewPosition < 0 || wrappedNewPosition >= songLengthQuarterNotes)
    {
        wrappedNewPosition = fmod(wrappedNewPosition, songLengthQuarterNotes);

        while (wrappedNewPosition < 0)
        {
            wrappedNewPosition += songLengthQuarterNotes;
        }
    }

    stepEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;

        const auto step = song->getStep(stepIndex);
        const auto sequence = getSequence(step.lock()->getSequence());

        if (sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          (sequence->getLastTick() * step.lock()->getRepeats());
        }

        const auto stepStartPositionQuarterNotes =
            ticksToQuarterNotes(stepStartTick);
        const auto stepEndPositionQuarterNotes =
            ticksToQuarterNotes(stepEndTick);

        if (wrappedNewPosition >= stepStartPositionQuarterNotes &&
            wrappedNewPosition < stepEndPositionQuarterNotes)
        {
            songScreen->setOffset(stepIndex - 1);

            const auto offsetWithinStepQuarterNotes =
                wrappedNewPosition - stepStartPositionQuarterNotes;

            const double finalPosQuarterNotes =
                fmod(offsetWithinStepQuarterNotes,
                     ticksToQuarterNotes(sequence->getLastTick()));

            if (finalPosQuarterNotes ==
                stateManager->getSnapshot().getPositionQuarterNotes())
            {
                return;
            }

            stateManager->enqueue(
                SetPositionQuarterNotes{finalPosQuarterNotes});
            stateManager->enqueue(
                SetPlayStartPositionQuarterNotes{finalPosQuarterNotes});
            break;
        }
    }
}

void Sequencer::moveWithinSong(const double positionQuarterNotesToUse)
{
    if (layeredScreen->getCurrentScreenName() != "song")
    {
        return;
    }

    const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    const auto song = songs[songScreen->getActiveSongIndex()];

    uint32_t stepStartTick;
    uint32_t stepEndTick = 0;
    uint64_t songEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;
        const auto step = song->getStep(stepIndex);
        const auto sequence = getSequence(step.lock()->getSequence());

        if (sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          (sequence->getLastTick() * step.lock()->getRepeats());
        }

        songEndTick = stepEndTick;
    }

    const double songLengthQuarterNotes = ticksToQuarterNotes(songEndTick);

    auto wrappedNewPosition = positionQuarterNotesToUse;

    if (wrappedNewPosition < 0 || wrappedNewPosition >= songLengthQuarterNotes)
    {
        wrappedNewPosition = fmod(wrappedNewPosition, songLengthQuarterNotes);

        while (wrappedNewPosition < 0)
        {
            wrappedNewPosition += songLengthQuarterNotes;
        }
    }

    stepEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;

        const auto step = song->getStep(stepIndex);
        const auto sequence = getSequence(step.lock()->getSequence());

        if (sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          (sequence->getLastTick() * step.lock()->getRepeats());
        }

        const auto stepStartPositionQuarterNotes =
            ticksToQuarterNotes(stepStartTick);
        const auto stepEndPositionQuarterNotes =
            ticksToQuarterNotes(stepEndTick);

        if (wrappedNewPosition >= stepStartPositionQuarterNotes &&
            wrappedNewPosition < stepEndPositionQuarterNotes)
        {
            songScreen->setOffset(stepIndex - 1);

            const auto offsetWithinStepQuarterNotes =
                wrappedNewPosition - stepStartPositionQuarterNotes;
            const auto offsetWithinStepTicks =
                quarterNotesToTicks(offsetWithinStepQuarterNotes);

            playedStepRepetitions =
                std::floor(offsetWithinStepTicks /
                           static_cast<float>(sequence->getLastTick()));
            sequence->resetTrackEventIndices(offsetWithinStepTicks %
                                             sequence->getLastTick());

            const double finalPosQuarterNotes =
                fmod(offsetWithinStepQuarterNotes,
                     ticksToQuarterNotes(sequence->getLastTick()));

            stateManager->enqueue(
                SetPositionQuarterNotes{finalPosQuarterNotes});
            stateManager->enqueue(
                SetPlayStartPositionQuarterNotes{finalPosQuarterNotes});
            break;
        }
    }
}

void Sequencer::move(const double positionQuarterNotesToUse)
{
    const auto songSequenceIndex = getSongSequenceIndex();
    const bool songMode = stateManager->getSnapshot().isSongModeEnabled();

    if (songMode && songSequenceIndex < 0)
    {
        return;
    }

    auto wrappedNewPosition = positionQuarterNotesToUse;

    const auto sequence = isPlaying() ? getCurrentlyPlayingSequence()
                                      : (songMode ? sequences[songSequenceIndex]
                                                  : getActiveSequence());

    const auto seqLengthQuarterNotes =
        ticksToQuarterNotes(sequence->getLastTick());

    if (wrappedNewPosition < 0 || wrappedNewPosition > seqLengthQuarterNotes)
    {
        wrappedNewPosition = fmod(wrappedNewPosition, seqLengthQuarterNotes);

        while (wrappedNewPosition < 0)
        {
            wrappedNewPosition += seqLengthQuarterNotes;
        }
    }

    stateManager->enqueue(SetPositionQuarterNotes{wrappedNewPosition});
    stateManager->enqueue(SetPlayStartPositionQuarterNotes{wrappedNewPosition});

    sequence->resetTrackEventIndices(quarterNotesToTicks(wrappedNewPosition));

    if (secondSequenceEnabled)
    {
        auto secondSequenceScreen =
            getScreens()->get<ScreenId::SecondSeqScreen>();
        sequences[secondSequenceScreen->sq]->resetTrackEventIndices(
            quarterNotesToTicks(wrappedNewPosition));
    }
}

int Sequencer::getTickPosition() const
{
    return stateManager->getSnapshot().getPositionTicks();
}

std::shared_ptr<Sequence> Sequencer::getCurrentlyPlayingSequence()
{
    auto seqIndex = getCurrentlyPlayingSequenceIndex();

    if (seqIndex == -1)
    {
        return {};
    }

    return sequences[seqIndex];
}

void Sequencer::setActiveTrackIndex(const int i)
{
    activeTrackIndex = i;
}

int Sequencer::getCurrentlyPlayingSequenceIndex() const
{
    const bool songMode = stateManager->getSnapshot().isSongModeEnabled();
    if (songMode)
    {
        auto songScreen = getScreens()->get<ScreenId::SongScreen>();
        auto song = songs[songScreen->getActiveSongIndex()];

        if (!song->isUsed())
        {
            return -1;
        }

        const auto seqIndexShouldBeDerivedFromSongStep =
            songMode && songScreen->getOffset() + 1 < song->getStepCount();

        auto songSeqIndex = seqIndexShouldBeDerivedFromSongStep
                                ? song->getStep(songScreen->getOffset() + 1)
                                      .lock()
                                      ->getSequence()
                                : -1;
        return songSeqIndex;
    }

    return currentlyPlayingSequenceIndex;
}

void Sequencer::setCurrentlyPlayingSequenceIndex(const int i)
{
    currentlyPlayingSequenceIndex = i;
    setActiveSequenceIndex(i);
}

int Sequencer::getNextSq() const
{
    return nextSq;
}

int Sequencer::getFirstUsedSeqDown(const int from, const bool unused) const
{
    for (int i = from; i >= 0; i--)
    {
        auto candidate =
            unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

        if (candidate)
        {
            return i;
        }
    }

    return -1;
}

int Sequencer::getFirstUsedSeqUp(const int from, const bool unused) const
{
    for (int i = from; i < 99; i++)
    {
        auto candidate =
            unused ? !sequences[i]->isUsed() : sequences[i]->isUsed();

        if (candidate)
        {
            return i;
        }
    }

    return -1;
}

void Sequencer::setNextSq(int i)
{
    if (i < -1)
    {
        i = -1;
    }
    if (i > 98)
    {
        i = 98;
    }

    auto startingFromScratch = nextSq == -1;

    auto up = i > nextSq;

    if (startingFromScratch)
    {
        up = i > currentlyPlayingSequenceIndex;
    }

    auto candidate = up ? getFirstUsedSeqUp(i) : getFirstUsedSeqDown(i);

    if (up && candidate == -1)
    {
        return;
    }

    nextSq = candidate;
}

void Sequencer::setNextSqPad(const int i)
{
    if (!sequences[i]->isUsed())
    {
        nextSq = -1;
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
    return stateManager->getSnapshot().isSongModeEnabled();
}

void Sequencer::setSongModeEnabled(const bool b)
{
    getStateManager()->enqueue(SetSongModeEnabled{b});
}

int Sequencer::getSongSequenceIndex() const
{
    auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    auto song = songs[songScreen->getActiveSongIndex()];
    auto step = songScreen->getOffset() + 1;

    if (step > song->getStepCount() - 1)
    {
        step = song->getStepCount() - 1;
    }

    if (step < 0)
    {
        return -1;
    }

    return song->getStep(step).lock()->getSequence();
}

bool Sequencer::isSecondSequenceEnabled() const
{
    return secondSequenceEnabled;
}

void Sequencer::setSecondSequenceEnabled(const bool b)
{
    secondSequenceEnabled = b;
}

void Sequencer::flushTrackNoteCache()
{
    for (auto &t : getCurrentlyPlayingSequence()->getTracks())
    {
        t->flushNoteCache();
    }
}

void Sequencer::storeActiveSequenceInUndoPlaceHolder()
{
    auto copy = copySequence(sequences[activeSequenceIndex]);
    undoPlaceHolder.swap(copy);

    undoSeqAvailable = true;

    hardware->getLed(mpc::hardware::ComponentId::UNDO_SEQ_LED)
        ->setEnabled(true);
}

void Sequencer::resetUndo()
{
    undoPlaceHolder.reset();
    undoSeqAvailable = false;
    hardware->getLed(mpc::hardware::ComponentId::UNDO_SEQ_LED)
        ->setEnabled(false);
}

bool Sequencer::isOverdubbing() const
{
    return overdubbing;
}

double Sequencer::getPlayStartPositionQuarterNotes() const
{
    return stateManager->getSnapshot().getPlayStartPositionQuarterNotes();
}

void Sequencer::setRecording(const bool b)
{
    recording = b;
}

void Sequencer::setOverdubbing(const bool b)
{
    overdubbing = b;
}

void Sequencer::playMetronomeTrack()
{
    if (isPlaying())
    {
        return;
    }

    metronomeOnly = true;
    frameSequencer->startMetronome();
}

void Sequencer::stopMetronomeTrack()
{
    if (!metronomeOnly)
    {
        return;
    }

    metronomeOnly = false;
    frameSequencer->stop();
}

std::shared_ptr<Sequence> Sequencer::createSeqInPlaceHolder()
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
    sequences[destIndex]->resetTrackEventIndices(quarterNotesToTicks(
        stateManager->getSnapshot().getPositionQuarterNotes()));
    clearPlaceHolder();
}

std::shared_ptr<Sequence> Sequencer::getPlaceHolder()
{
    return placeHolder;
}

int Sequencer::getPlayedStepRepetitions() const
{
    return playedStepRepetitions;
}

void Sequencer::setEndOfSong(const bool b)
{
    endOfSong = b;
}

void Sequencer::incrementPlayedStepRepetitions()
{
    playedStepRepetitions++;
}

void Sequencer::resetPlayedStepRepetitions()
{
    playedStepRepetitions = 0;
}

bool Sequencer::isPunchEnabled() const
{
    return punchEnabled;
}

void Sequencer::setPunchEnabled(const bool enabled)
{
    punchEnabled = enabled;
}

int Sequencer::getAutoPunchMode() const
{
    return autoPunchMode;
}

void Sequencer::setAutoPunchMode(const int mode)
{
    if (mode >= 0 && mode <= 2)
    {
        autoPunchMode = mode;
    }
}

int Sequencer::getPunchInTime() const
{
    return punchInTime;
}

void Sequencer::setPunchInTime(const int time)
{
    punchInTime = time;
}

int Sequencer::getPunchOutTime() const
{
    return punchOutTime;
}

void Sequencer::setPunchOutTime(const int time)
{
    punchOutTime = time;
}

template <typename T> std::shared_ptr<T> Sequencer::getBus(const int busIndex)
{
    if (busIndex < 0 || busIndex >= buses.size())
    {
        return {};
    }

    auto result = std::dynamic_pointer_cast<T>(buses[busIndex]);
    return result;
}

std::shared_ptr<DrumBus> Sequencer::getDrumBus(const int drumBusIndex) const
{
    assert(drumBusIndex >= 0 || drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT);
    auto result = std::dynamic_pointer_cast<DrumBus>(buses[drumBusIndex + 1]);
    assert(result);
    return result;
}

std::shared_ptr<FrameSeq> Sequencer::getFrameSequencer()
{
    return frameSequencer;
}

template std::shared_ptr<Bus> Sequencer::getBus(const int busIndex);
template std::shared_ptr<MidiBus> Sequencer::getBus(const int busIndex);
template std::shared_ptr<DrumBus> Sequencer::getBus(const int busIndex);
