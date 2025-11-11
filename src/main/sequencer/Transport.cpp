#include "Transport.hpp"
#include "Sequencer.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/TempoChangeEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

Transport::Transport(Sequencer &owner) : sequencer(owner)
{
    const auto userScreen = sequencer.getScreens()->get<ScreenId::UserScreen>();
    tempo = userScreen->getTempo();
}

bool Transport::isPlaying() const
{
    return !metronomeOnlyEnabled && sequencer.getFrameSequencer()->isRunning();
}

void Transport::play(const bool fromStart)
{
    if (isPlaying())
    {
        return;
    }

    endOfSong = false;
    const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();
    const auto currentSong =
        sequencer.getSong(songScreen->getActiveSongIndex());

    const auto snapshot = sequencer.getStateManager()->getSnapshot();
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
            !sequencer.getSequence(currentStep->getSequence())->isUsed())
        {
            return;
        }
    }

    const auto countMetronomeScreen =
        sequencer.getScreens()->get<ScreenId::CountMetronomeScreen>();
    const auto countInMode = countMetronomeScreen->getCountInMode();

    std::optional<int64_t> positionQuarterNotesToStartPlayingFrom = std::nullopt;

    if (!countEnabled || countInMode == 0 ||
        (countInMode == 1 && !isRecordingOrOverdubbing()))
    {
        if (fromStart && snapshot.getPositionQuarterNotes() != 0)
        {
            positionQuarterNotesToStartPlayingFrom = 0;
        }
    }

    const auto s = sequencer.getActiveSequence();

    if (countEnabled && !songMode)
    {
        if (countInMode == 2 ||
            (countInMode == 1 && isRecordingOrOverdubbing()))
        {
            if (fromStart)
            {
                positionQuarterNotesToStartPlayingFrom = Sequencer::ticksToQuarterNotes(s->getLoopStart());
            }
            else
            {
                positionQuarterNotesToStartPlayingFrom = Sequencer::ticksToQuarterNotes(
                    s->getFirstTickOfBar(getCurrentBarIndex()));
            }

            countInStartPos =
                Sequencer::quarterNotesToTicks(snapshot.getPositionQuarterNotes());
            countInEndPos = s->getLastTickOfBar(getCurrentBarIndex());

            countingIn = true;
        }
    }

    if (positionQuarterNotesToStartPlayingFrom)
    {
        setPosition(*positionQuarterNotesToStartPlayingFrom);
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
            sequencer.storeActiveSequenceInUndoPlaceHolder();
        }
    }

    if (sequencer.isBouncePrepared())
    {
        sequencer.startBouncing();
    }
    else
    {
        sequencer.getFrameSequencer()->start();
    }
}

void Transport::playFromStart()
{
    if (isPlaying())
    {
        return;
    }

    play(true);
}

void Transport::play()
{
    if (isPlaying())
    {
        return;
    }

    play(false);
}

void Transport::rec()
{
    if (isPlaying())
    {
        return;
    }

    recording = true;

    play(false);
}

void Transport::recFromStart()
{
    if (isPlaying())
    {
        return;
    }

    recording = true;
    play(true);
}

void Transport::overdub()
{
    if (isPlaying())
    {
        return;
    }

    overdubbing = true;
    play(false);
}

void Transport::switchRecordToOverdub()
{
    if (!recording)
    {
        return;
    }

    recording = false;
    overdubbing = true;

    // Maybe we should give our Led GUI component knowledge of Transport
    // so it can figure this out itself. It's already polling other kinds
    // of state.
    sequencer.hardware->getLed(hardware::ComponentId::REC_LED)
        ->setEnabled(false);
    sequencer.hardware->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(true);
}

void Transport::overdubFromStart()
{
    if (isPlaying())
    {
        return;
    }

    overdubbing = true;
    play(true);
}

void Transport::stop()
{
    stop(AT_START_OF_BUFFER);
}

void Transport::stop(const StopMode stopMode)
{
    const bool bouncing = sequencer.isBouncing();

    if (!isPlaying() && !bouncing)
    {

        if (const auto snapshot = sequencer.getStateManager()->getSnapshot();
            snapshot.getPositionQuarterNotes() != 0.0)
        {
            setPosition(0); // real 2kxl doesn't do this
        }

        return;
    }

    playedStepRepetitions = 0;
    sequencer.getStateManager()->enqueue(SetSongModeEnabled{false});
    sequencer.setNextSq(-1);

    const auto activeSequence = sequencer.getActiveSequence();
    const auto pos = getTickPosition();

    int64_t newTickPosition = pos;

    if (pos > activeSequence->getLastTick())
    {
        newTickPosition = activeSequence->getLastTick();
    }

    // const int frameOffset = stopMode == AT_START_OF_BUFFER
    //                             ? 0
    //                             : frameSequencer->getEventFrameOffset();

    sequencer.getFrameSequencer()->stop();

    recording = false;
    overdubbing = false;

    if (countingIn)
    {
        newTickPosition = countInStartPos;
        resetCountInPositions();
        countingIn = false;
    }

    setPosition(Sequencer::ticksToQuarterNotes(newTickPosition));

    const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();

    if (endOfSong)
    {
        songScreen->setOffset(songScreen->getOffset() + 1);
    }

    const auto vmpcDirectToDiskRecorderScreen =
        sequencer.getScreens()->get<ScreenId::VmpcDirectToDiskRecorderScreen>();

    if (bouncing && vmpcDirectToDiskRecorderScreen->getRecord() != 4)
    {
        sequencer.stopBouncing();
    }

    sequencer.hardware->getLed(hardware::ComponentId::PLAY_LED)
        ->setEnabled(false);
    sequencer.hardware->getLed(hardware::ComponentId::REC_LED)
        ->setEnabled(false);
    sequencer.hardware->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(false);
}

void Transport::resetCountInPositions()
{
    countInStartPos = -1;
    countInEndPos = -1;
}

void Transport::setRecording(const bool b)
{
    recording = b;
}

void Transport::setOverdubbing(const bool b)
{
    overdubbing = b;
}

void Transport::setCountEnabled(const bool b)
{
    countEnabled = b;
}

bool Transport::isCountEnabled() const
{
    return countEnabled;
}

void Transport::setCountingIn(const bool b)
{
    countingIn = b;

    if (!countingIn)
    {
        resetCountInPositions();
    }
}

bool Transport::isCountingIn() const
{
    return countingIn;
}

void Transport::bumpPositionByTicks(const uint8_t tickCount) const
{
    sequencer.getStateManager()->enqueue(BumpPositionByTicks{tickCount});
}

int Transport::getTickPosition() const
{
    return sequencer.getStateManager()->getSnapshot().getPositionTicks();
}

double Transport::getPlayStartPositionQuarterNotes() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getPlayStartPositionQuarterNotes();
}

void Transport::playMetronomeTrack()
{
    if (isPlaying())
    {
        return;
    }

    metronomeOnlyEnabled = true;
    sequencer.getFrameSequencer()->startMetronome();
}

void Transport::stopMetronomeTrack()
{
    if (!metronomeOnlyEnabled)
    {
        return;
    }

    sequencer.getFrameSequencer()->stop();
    metronomeOnlyEnabled = false;
}

bool Transport::isMetronomeOnlyEnabled() const
{
    return metronomeOnlyEnabled;
}

int Transport::getPlayedStepRepetitions() const
{
    return playedStepRepetitions;
}

void Transport::setEndOfSong(const bool b)
{
    endOfSong = b;
}

void Transport::incrementPlayedStepRepetitions()
{
    playedStepRepetitions++;
}

void Transport::resetPlayedStepRepetitions()
{
    playedStepRepetitions = 0;
}

bool Transport::isPunchEnabled() const
{
    return punchEnabled;
}

void Transport::setPunchEnabled(const bool enabled)
{
    punchEnabled = enabled;
}

int Transport::getAutoPunchMode() const
{
    return autoPunchMode;
}

void Transport::setAutoPunchMode(const int mode)
{
    if (mode >= 0 && mode <= 2)
    {
        autoPunchMode = mode;
    }
}

int Transport::getPunchInTime() const
{
    return punchInTime;
}

void Transport::setPunchInTime(const int time)
{
    punchInTime = time;
}

int Transport::getPunchOutTime() const
{
    return punchOutTime;
}

void Transport::setPunchOutTime(const int time)
{
    punchOutTime = time;
}

int64_t Transport::getCountInStartPos() const
{
    return countInStartPos;
}

int64_t Transport::getCountInEndPos() const
{
    return countInEndPos;
}

bool Transport::isTempoSourceSequenceEnabled() const
{
    return tempoSourceSequenceEnabled;
}

void Transport::setTempoSourceSequence(const bool b)
{
    tempoSourceSequenceEnabled = b;
}

bool Transport::isRecordingOrOverdubbing() const
{
    return recording || overdubbing;
}

bool Transport::isRecording() const
{
    return recording;
}

bool Transport::isOverdubbing() const
{
    return overdubbing;
}

int Transport::getCurrentBarIndex() const
{
    const auto s = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                               : sequencer.getActiveSequence();
    const auto pos =
        isCountingIn()
            ? Sequencer::quarterNotesToTicks(getPlayStartPositionQuarterNotes())
            : getTickPosition();

    if (pos == s->getLastTick())
    {
        return s->getLastBarIndex() + 1;
    }

    const auto &barLengths = s->getBarLengthsInTicks();

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

int Transport::getCurrentBeatIndex() const
{
    const auto s = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                               : sequencer.getActiveSequence();
    const auto pos =
        isCountingIn()
            ? Sequencer::quarterNotesToTicks(getPlayStartPositionQuarterNotes())
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

    const auto ts = s->getTimeSignature();
    const auto den = ts.getDenominator();
    const auto denTicks = 96 * (4.0 / den);

    if (index == 0)
    {
        return 0;
    }

    int barStartPos = 0;
    auto barCounter = 0;

    const auto currentBarIndex = getCurrentBarIndex();

    for (const auto &l : s->getBarLengthsInTicks())
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

int Transport::getCurrentClockNumber() const
{
    const auto sequence = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                                      : sequencer.getActiveSequence();

    auto clock =
        isCountingIn()
            ? Sequencer::quarterNotesToTicks(getPlayStartPositionQuarterNotes())
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

    const auto ts = sequence->getTimeSignature();
    const auto den = ts.getDenominator();
    const auto denTicks = 96 * (4.0 / den);

    if (clock == 0)
    {
        return 0;
    }

    auto barCounter = 0;
    const auto currentBarIndex = getCurrentBarIndex();

    for (const auto &l : sequence->getBarLengthsInTicks())
    {
        if (barCounter == currentBarIndex)
        {
            break;
        }

        clock -= l;
        barCounter++;
    }

    const auto currentBeatIndex = getCurrentBeatIndex();

    for (int i = 0; i < currentBeatIndex; i++)
    {
        clock -= denTicks;
    }

    return clock;
}

void Transport::setBarBeatClock(const int bar, const int beat, const int clock) const
{
    if (isPlaying())
    {
        return;
    }

    const auto s = sequencer.getActiveSequence();
    const auto &barLengths = s->getBarLengthsInTicks();
    const auto ts = s->getTimeSignature();

    const int clampedBar  = std::clamp(bar,  0, static_cast<int>(barLengths.size()) - 1);
    const int clampedBeat = std::clamp(beat, 0, ts.getNumerator() - 1);

    const int denTicks = static_cast<int>(96 * (4.0 / ts.getDenominator()));
    const int clampedClock = std::clamp(clock, 0, denTicks - 1);

    int pos = 0;
    for (int b = 0; b < clampedBar; ++b)
    {
        pos += barLengths[b];
    }

    pos += clampedBeat * denTicks + clampedClock;

    setPosition(Sequencer::ticksToQuarterNotes(pos));
}

void Transport::setBar(int i) const
{
    if (isPlaying())
    {
        return;
    }

    const auto s = sequencer.getActiveSequence();
    const auto &barLengths = s->getBarLengthsInTicks();

    i = std::clamp(i, 0, static_cast<int>(barLengths.size()) - 1);

    int pos = 0;
    for (int b = 0; b < i; ++b)
    {
        pos += barLengths[b];
    }

    setPosition(Sequencer::ticksToQuarterNotes(pos));
}

void Transport::setBeat(int i) const
{
    if (isPlaying())
    {
        return;
    }

    if (i < 0)
    {
        i = 0;
    }

    const auto s = sequencer.getActiveSequence();
    auto pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    const auto ts = s->getTimeSignature();

    if (const auto num = ts.getNumerator(); i >= num)
    {
        i = num - 1;
    }

    const auto difference = i - getCurrentBeatIndex();

    const auto denTicks = 96 * (4.0 / ts.getDenominator());
    pos += difference * denTicks;
    setPosition(Sequencer::ticksToQuarterNotes(pos));
}

void Transport::setClock(int i) const
{
    if (isPlaying())
    {
        return;
    }

    if (i < 0)
    {
        i = 0;
    }

    const auto s = sequencer.getActiveSequence();
    int pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    const auto den = s->getTimeSignature().getDenominator();

    if (const auto denTicks = 96 * (4.0 / den); i > denTicks - 1)
    {
        i = denTicks - 1;
    }

    const int difference = i - getCurrentClockNumber();

    pos += difference;
    setPosition(Sequencer::ticksToQuarterNotes(pos));
}

void Transport::setPosition(const double positionQuarterNotes,
                            const bool shouldSyncTrackEventIndicesToNewPosition,
                            const bool shouldSetPlayStartPosition) const
{
    const auto songSequenceIndex = sequencer.getSongSequenceIndex();
    const bool songMode =
        sequencer.getStateManager()->getSnapshot().isSongModeEnabled();

    if (songMode && songSequenceIndex == -1)
    {
        return;
    }

    const auto sequence = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                          : songMode  ? sequencer.getSequence(songSequenceIndex)
                                      : sequencer.getActiveSequence();

    const auto seqLengthQuarterNotes =
        Sequencer::ticksToQuarterNotes(sequence->getLastTick());
    auto wrappedNewPosition = positionQuarterNotes;

    if (wrappedNewPosition < 0 || wrappedNewPosition >= seqLengthQuarterNotes)
    {
        wrappedNewPosition = fmod(wrappedNewPosition, seqLengthQuarterNotes);
        while (wrappedNewPosition < 0)
        {
            wrappedNewPosition += seqLengthQuarterNotes;
        }
    }

    sequencer.getStateManager()->enqueue(
        SetPositionQuarterNotes{wrappedNewPosition});

    if (shouldSetPlayStartPosition)
    {
        sequencer.getStateManager()->enqueue(
            SetPlayStartPositionQuarterNotes{wrappedNewPosition});
    }

    if (shouldSyncTrackEventIndicesToNewPosition)
    {
        sequence->syncTrackEventIndices(
            Sequencer::quarterNotesToTicks(wrappedNewPosition));
        if (sequencer.isSecondSequenceEnabled())
        {
            const auto secondSequenceScreen =
                sequencer.getScreens()->get<ScreenId::SecondSeqScreen>();
            sequencer.getSequence(secondSequenceScreen->getSq())
                ->syncTrackEventIndices(
                    Sequencer::quarterNotesToTicks(wrappedNewPosition));
        }
    }
}

void Transport::setPositionWithinSong(
    const double positionQuarterNotes,
    const bool shouldSyncTrackEventIndicesToNewPosition,
    const bool shouldSetPlayStartPosition)
{
    if (!screengroups::isSongScreen(
            sequencer.layeredScreen->getCurrentScreen()))
    {
        return;
    }

    const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();
    const auto song = sequencer.getSong(songScreen->getActiveSongIndex());
    uint32_t stepStartTick = 0;
    uint32_t stepEndTick = 0;
    uint64_t songEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;
        const auto step = song->getStep(stepIndex);

        if (const auto sequence =
                sequencer.getSequence(step.lock()->getSequence());
            sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          sequence->getLastTick() * step.lock()->getRepeats();
        }
        songEndTick = stepEndTick;
    }

    const double songLengthQuarterNotes =
        Sequencer::ticksToQuarterNotes(songEndTick);
    auto wrappedNewPosition = positionQuarterNotes;

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
        const auto sequence = sequencer.getSequence(step.lock()->getSequence());

        if (sequence->isUsed())
        {
            stepEndTick = stepStartTick +
                          sequence->getLastTick() * step.lock()->getRepeats();
        }

        const auto stepStartPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepStartTick);
        const auto stepEndPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepEndTick);

        if (wrappedNewPosition >= stepStartPositionQuarterNotes &&
            wrappedNewPosition < stepEndPositionQuarterNotes)
        {
            songScreen->setOffset(stepIndex - 1);
            const auto offsetWithinStepQuarterNotes =
                wrappedNewPosition - stepStartPositionQuarterNotes;
            const double finalPosQuarterNotes =
                fmod(offsetWithinStepQuarterNotes,
                     Sequencer::ticksToQuarterNotes(sequence->getLastTick()));

            if (finalPosQuarterNotes == sequencer.getStateManager()
                                            ->getSnapshot()
                                            .getPositionQuarterNotes())
            {
                return;
            }

            setPosition(finalPosQuarterNotes,
                        shouldSyncTrackEventIndicesToNewPosition,
                        shouldSetPlayStartPosition);

            if (shouldSyncTrackEventIndicesToNewPosition)
            {
                const auto offsetWithinStepTicks =
                    Sequencer::quarterNotesToTicks(
                        offsetWithinStepQuarterNotes);
                playedStepRepetitions =
                    std::floor(offsetWithinStepTicks /
                               static_cast<float>(sequence->getLastTick()));
                sequence->syncTrackEventIndices(offsetWithinStepTicks %
                                                 sequence->getLastTick());
            }
            break;
        }
    }
}

void Transport::setTempo(double newTempo)
{
    if (newTempo < 30.0)
    {
        newTempo = 30.0;
    }
    else if (newTempo > 300.0)
    {
        newTempo = 300.0;
    }

    const auto s = sequencer.getActiveSequence();
    const auto tce = sequencer.getCurrentTempoChangeEvent();

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
        const auto initialTempo = s->getInitialTempo();
        const auto ratio = newTempo / initialTempo;
        tce->setRatio(static_cast<int>(round(ratio * 1000.0)));
    }
    else
    {
        s->setInitialTempo(newTempo);
    }
}

double Transport::getTempo() const
{
    if (!isPlaying() && !sequencer.getActiveSequence()->isUsed())
    {
        return tempo;
    }

    const auto seq = sequencer.getActiveSequence();

    if (screengroups::isSongScreen(sequencer.layeredScreen->getCurrentScreen()))
    {
        if (!seq->isUsed())
        {
            return 120.0;
        }
    }

    const auto tce = sequencer.getCurrentTempoChangeEvent();

    if (tempoSourceSequenceEnabled)
    {
        const auto ignoreTempoChangeScreen =
            sequencer.getScreens()->get<ScreenId::IgnoreTempoChangeScreen>();

        if (seq->isTempoChangeOn() ||
            (sequencer.getStateManager()->getSnapshot().isSongModeEnabled() &&
             !ignoreTempoChangeScreen->getIgnore()))
        {
            if (tce)
            {
                return tce->getTempo();
            }
        }

        return sequencer.getActiveSequence()->getInitialTempo();
    }

    if (seq->isTempoChangeOn() && tce)
    {
        return tempo * tce->getRatio() * 0.001;
    }

    return tempo;
}
