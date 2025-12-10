#include "Transport.hpp"

#include "FloatTypes.hpp"
#include "SeqUtil.hpp"
#include "SequencerStateManager.hpp"
#include "SequenceStateView.hpp"
#include "Sequencer.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/TempoChangeEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;
using namespace mpc::lcdgui;

Transport::Transport(Sequencer &owner) : sequencer(owner)
{
    const auto userScreen = sequencer.getScreens()->get<ScreenId::UserScreen>();
    setMasterTempo(userScreen->getTempo());
}

bool Transport::isPlaying() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isSequencerRunning();
}

bool Transport::isPlayPossible(const bool fromStart) const
{
    if (sequencer.isSongModeEnabled())
    {
        const auto song = sequencer.getSelectedSong();

        if (!song->isUsed())
        {
            return false;
        }

        if (sequencer.isSelectedSongStepIndexEndOfSong() && !fromStart)
        {
            return false;
        }

        const SongStepIndex stepIndex =
            fromStart ? MinSongStepIndex : sequencer.getSelectedSongStepIndex();

        if (const auto step = song->getStep(stepIndex);
            !sequencer.getSequence(step.sequenceIndex)->isUsed())
        {
            return false;
        }
    }
    else
    {
        if (!sequencer.getSelectedSequence()->isUsed())
        {
            return false;
        }
    }

    return true;
}

void Transport::play(const bool fromStart) const
{
    if (!isPlayPossible(fromStart))
    {
        return;
    }

    const bool isSongMode = sequencer.isSongModeEnabled();
    const auto stateManager = sequencer.getStateManager();

    if (fromStart)
    {
        if (isSongMode)
        {
            stateManager->enqueue(PlaySongFromStart{});
        }
        else
        {
            stateManager->enqueue(PlaySequenceFromStart{});
        }
    }
    else
    {
        if (isSongMode)
        {
            stateManager->enqueue(PlaySong{});
        }
        else
        {
            stateManager->enqueue(PlaySequence{});
        }
    }
}

void Transport::playImmediately() const
{
    if (!isPlayPossible(false))
    {
        return;
    }

    const bool isSongMode = sequencer.isSongModeEnabled();
    const auto stateManager = sequencer.getStateManager();

    if (isSongMode)
    {
        stateManager->applyMessageImmediate(PlaySong{});
    }
    else
    {
        stateManager->applyMessageImmediate(PlaySequence{});
    }
}

void Transport::rec() const
{
    if (isPlaying())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(Record{});
}

void Transport::recFromStart() const
{
    if (isPlaying())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(RecordFromStart{});
}

void Transport::overdub() const
{
    if (isPlaying())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(Overdub{});
}

void Transport::switchRecordToOverdub() const
{
    if (!isRecording())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(SwitchRecordToOverdub{});
}

void Transport::overdubFromStart() const
{
    if (isPlaying())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(OverdubFromStart{});
}

void Transport::stop() const
{
    if (sequencer.isSongModeEnabled())
    {
        sequencer.getStateManager()->enqueue(StopSong{});
    }
    else
    {
        sequencer.getStateManager()->enqueue(StopSequence{});
    }
    sequencer.flushMidiNoteOffs();
}

void Transport::resetCountInPositions() const
{
    sequencer.getStateManager()->enqueue(SetCountInPositions{NoTick, NoTick});
}

void Transport::setRecording(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetRecordingEnabled{b});
}

void Transport::setOverdubbing(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetOverdubbingEnabled{b});
}

mpc::PositionQuarterNotes Transport::getWrappedPositionInSequence(
    const PositionQuarterNotes positionQuarterNotes) const
{
    const bool songMode = sequencer.isSongModeEnabled();

    SequenceIndex songSequenceIndex;

    if (songMode)
    {
        songSequenceIndex = sequencer.getSongSequenceIndex();

        if (songSequenceIndex == NoSequenceIndex)
        {
            return NoPositionQuarterNotes;
        }
    }

    const auto sequence = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                          : songMode  ? sequencer.getSequence(songSequenceIndex)
                                      : sequencer.getSelectedSequence();

    const auto seqLengthQuarterNotes =
        Sequencer::ticksToQuarterNotes(sequence->getLastTick());

    PositionQuarterNotes result = positionQuarterNotes;

    if (seqLengthQuarterNotes == 0)
    {
        result = 0;
    }
    else if (result < 0 || result >= seqLengthQuarterNotes)
    {
        result = fmod(result, seqLengthQuarterNotes);
        while (result < 0)
        {
            result += seqLengthQuarterNotes;
        }
    }

    return result;
}

WrappedSongPosition Transport::getWrappedPositionInSong(
    const PositionQuarterNotes positionQuarterNotes) const
{
    WrappedSongPosition result{NoPositionQuarterNotes, SongStepIndex(0), 0};

    const auto song = sequencer.getSelectedSong();
    if (!song || song->getStepCount() == 0)
    {
        return result;
    }

    uint64_t songEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); ++stepIndex)
    {
        const auto step = song->getStep(SongStepIndex(stepIndex));
        const auto sequence = sequencer.getSequence(step.sequenceIndex);
        if (!sequence->isUsed())
        {
            continue;
        }

        const uint64_t seqTicks = sequence->getLastTick();
        songEndTick += seqTicks * step.repetitionCount;
    }

    const double songLengthQuarterNotes =
        Sequencer::ticksToQuarterNotes(songEndTick);
    if (songLengthQuarterNotes <= 0.0)
    {
        return result;
    }

    auto wrapped = positionQuarterNotes;

    if (wrapped < 0.0 || wrapped >= songLengthQuarterNotes)
    {
        wrapped = std::fmod(wrapped, songLengthQuarterNotes);
        if (wrapped < 0.0)
        {
            wrapped += songLengthQuarterNotes;
        }
    }

    uint64_t stepStartTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); ++stepIndex)
    {
        const auto step = song->getStep(SongStepIndex(stepIndex));
        const auto sequence = sequencer.getSequence(step.sequenceIndex);
        if (!sequence->isUsed())
        {
            continue;
        }

        const uint64_t seqTicks = sequence->getLastTick();
        const uint64_t stepTicks = seqTicks * step.repetitionCount;
        const uint64_t stepEndTick = stepStartTick + stepTicks;

        const double stepStartQN =
            Sequencer::ticksToQuarterNotes(stepStartTick);
        const double stepEndQN = Sequencer::ticksToQuarterNotes(stepEndTick);

        if (wrapped >= stepStartQN && wrapped < stepEndQN)
        {
            const double offsetWithinStepQN = wrapped - stepStartQN;
            const double seqLengthQN = Sequencer::ticksToQuarterNotes(seqTicks);

            int playedRepetitionCount = 0;
            if (seqLengthQN > 0.0)
            {
                playedRepetitionCount =
                    static_cast<int>(offsetWithinStepQN / seqLengthQN);
                if (playedRepetitionCount >= step.repetitionCount)
                {
                    playedRepetitionCount = step.repetitionCount - 1;
                }
            }

            result.position = fmod(offsetWithinStepQN, seqLengthQN);
            result.stepIndex = SongStepIndex(stepIndex);
            result.playedRepetitionCount = playedRepetitionCount;
            break;
        }

        stepStartTick = stepEndTick;
    }

    return result;
}

void Transport::moveSongToStepThatContainsPosition(
    const PositionQuarterNotes positionQuarterNotes) const
{
    const auto song = sequencer.getSelectedSong();

    uint32_t stepStartTick = 0, stepEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); ++stepIndex)
    {
        stepStartTick = stepEndTick;

        const auto step = song->getStep(SongStepIndex(stepIndex));

        if (const auto sequence = sequencer.getSequence(step.sequenceIndex);
            sequence->isUsed())
        {
            stepEndTick =
                stepStartTick + sequence->getLastTick() * step.repetitionCount;
        }

        const auto stepStartPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepStartTick);
        const auto stepEndPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepEndTick);

        if (positionQuarterNotes >= stepStartPositionQuarterNotes &&
            positionQuarterNotes < stepEndPositionQuarterNotes)
        {
            sequencer.setSelectedSongStepIndex(SongStepIndex(stepIndex));
            break;
        }
    }
}

void Transport::setCountEnabled(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetCountEnabled{b});
}

bool Transport::isCountEnabled() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isCountEnabled();
}

void Transport::setCountingIn(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetCountingIn{b});

    if (!b)
    {
        resetCountInPositions();
    }
}

bool Transport::isCountingIn() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isCountingIn();
}

int Transport::getTickPosition() const
{
    const auto snapshot = sequencer.getStateManager()->getSnapshot();
    const auto transportState = snapshot.getTransportStateView();
    return transportState.getPositionTicks();
}

mpc::Tick Transport::getTickPositionGuiPresentation() const
{
    if (isCountingIn())
    {
        return getCountInStartPosTicks();
    }

    return getTickPosition();
}

double Transport::getPositionQuarterNotes() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .getPositionQuarterNotes();
}

double Transport::getPlayStartPositionQuarterNotes() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .getPlayStartPositionQuarterNotes();
}

void Transport::playMetronomeOnly() const
{
    if (isPlaying())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(PlayMetronomeOnly{});
}

void Transport::stopMetronomeOnly() const
{
    if (!isPlaying() || !isMetronomeOnlyEnabled())
    {
        return;
    }

    sequencer.getStateManager()->enqueue(StopMetronomeOnly{});
}

bool Transport::isMetronomeOnlyEnabled() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isMetronomeOnlyEnabled();
}

int Transport::getPlayedStepRepetitions() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .getPlayedSongStepRepetitionCount();
}

void Transport::setEndOfSong(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetEndOfSongEnabled{b});
}

void Transport::incrementPlayedStepRepetitions() const
{
    sequencer.getStateManager()->enqueue(
        SetPlayedSongStepRepetitionCount{getPlayedStepRepetitions() + 1});
}

void Transport::resetPlayedStepRepetitions() const
{
    sequencer.getStateManager()->enqueue(SetPlayedSongStepRepetitionCount{0});
}

void Transport::bumpPositionByTicks(const Tick ticks) const
{
    const auto snapshot =
        sequencer.getStateManager()->getSnapshot().getTransportStateView();
    const auto pos = snapshot.getPositionTicks();
    const auto newPos = Sequencer::ticksToQuarterNotes(pos + ticks);
    setPosition(newPos);
}

void Transport::setMasterTempo(const double masterTempo) const
{
    sequencer.getStateManager()->enqueue(
        SetMasterTempo{std::clamp(masterTempo, 30.0, 300.0)});
}

double Transport::getMasterTempo() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .getMasterTempo();
}

void Transport::setShouldWaitForMidiClockLock(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetShouldWaitForMidiClockLock{b});
}

bool Transport::shouldWaitForMidiClockLock() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isShouldWaitForMidiClockLockEnabled();
}

void Transport::setPositionTicksToReturnToWhenReleasingRec(
    const Tick tick) const
{
    sequencer.getStateManager()->enqueue(
        SetPositionTicksToReturnToWhenReleaseRec{tick});
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

int64_t Transport::getCountInStartPosTicks() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .countInStartPosTicks();
}

int64_t Transport::getCountInEndPosTicks() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .countInEndPosTicks();
}

bool Transport::isTempoSourceSequence() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isTempoSourceSequenceEnabled();
}

void Transport::setTempoSourceIsSequence(const bool b) const
{
    sequencer.getStateManager()->enqueue(SetTempoSourceIsSequence{b});
}

bool Transport::isRecordingOrOverdubbing() const
{
    return isRecording() || isOverdubbing();
}

bool Transport::isRecording() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isRecording();
}

bool Transport::isOverdubbing() const
{
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isOverdubbing();
}

int Transport::getCurrentBarIndex() const
{
    const auto seq = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                                 : sequencer.getSelectedSequence();

    if (!seq)
    {
        return 0;
    }

    const auto pos = getTickPositionGuiPresentation();

    return seq->getBarIndexForPositionTicks(pos);
}

int Transport::getCurrentBeatIndex() const
{
    const auto seq = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                                 : sequencer.getSelectedSequence();

    if (!seq)
    {
        return 0;
    }

    const auto pos = getTickPositionGuiPresentation();

    if (pos == seq->getLastTick())
    {
        return 0;
    }

    auto index = pos;

    if (isPlaying() && !isCountingIn())
    {
        index = getTickPosition();

        if (index > seq->getLastTick())
        {
            index %= seq->getLastTick();
        }
    }

    const auto den = seq->getTimeSignatureFromTickPos(pos).denominator;
    const auto denTicks = 96 * (4.0 / den);

    if (index == 0)
    {
        return 0;
    }

    int barStartPos = 0;

    const auto currentBarIndex = getCurrentBarIndex();

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (i == currentBarIndex)
        {
            break;
        }

        barStartPos += seq->getBarLength(i);
    }

    const auto beatIndex =
        static_cast<int>(floor((index - barStartPos) / denTicks));
    return beatIndex;
}

int Transport::getCurrentClockNumber() const
{
    const auto sequence = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                                      : sequencer.getSelectedSequence();

    if (!sequence)
    {
        return 0;
    }

    const auto tickPos = getTickPositionGuiPresentation();
    return SeqUtil::getClock(sequence.get(), tickPos);
}

void Transport::setBarBeatClock(const int bar, const int beat,
                                const int clock) const
{
    if (isPlaying())
    {
        return;
    }

    const auto seq = sequencer.getSelectedSequence();

    const auto [num, den] = seq->getTimeSignatureFromTickPos(getTickPosition());

    const int clampedBar = std::clamp(
        bar, 0, static_cast<int>(Mpc2000XlSpecs::MAX_LAST_BAR_INDEX));

    const int clampedBeat = std::clamp(beat, 0, static_cast<int>(num) - 1);

    const int denTicks = static_cast<int>(96 * (4.0 / den));
    const int clampedClock = std::clamp(clock, 0, denTicks - 1);

    int pos = 0;
    for (int b = 0; b < clampedBar; ++b)
    {
        pos += seq->getBarLength(b);
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

    const auto seq = sequencer.getSelectedSequence();

    i = std::clamp(
        i, 0,
        std::min(seq->getBarCount(),
                 static_cast<int>(Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)));

    int pos = 0;

    for (int b = 0; b < i; ++b)
    {
        pos += seq->getBarLength(b);
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

    const auto s = sequencer.getSelectedSequence();
    auto pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    const auto [num, den] = s->getTimeSignatureFromTickPos(pos);

    if (i >= num)
    {
        i = num - 1;
    }

    const auto difference = i - getCurrentBeatIndex();

    const auto denTicks = 96 * (4.0 / den);
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

    const auto s = sequencer.getSelectedSequence();
    int pos = getTickPosition();

    if (pos == s->getLastTick())
    {
        return;
    }

    const auto den = s->getTimeSignatureFromTickPos(pos).denominator;

    if (const auto denTicks = 96 * (4.0 / den); i > denTicks - 1)
    {
        i = denTicks - 1;
    }

    const int difference = i - getCurrentClockNumber();

    pos += difference;
    setPosition(Sequencer::ticksToQuarterNotes(pos));
}

void Transport::setPosition(const double positionQuarterNotes) const
{
    const auto stateManager = sequencer.getStateManager();

    stateManager->enqueue(SetPositionQuarterNotes{positionQuarterNotes});

    if (!isPlaying())
    {
        stateManager->enqueue(SyncTrackEventIndices{
            stateManager->getSnapshot().getSelectedSequenceIndex()});
    }
}

void Transport::setTempo(const double newTempo) const
{
    const auto s = sequencer.getSelectedSequence();
    const auto tce = sequencer.getCurrentTempoChangeEvent();

    if (!s || !s->isUsed() || !isTempoSourceSequence())
    {
        if (!tce)
        {
            setMasterTempo(newTempo);
            return;
        }

        const auto tempoWithRatioApplied = newTempo / (tce->getRatio() * 0.001);

        setMasterTempo(tempoWithRatioApplied);

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
    const auto sequencerSnapshot = sequencer.getStateManager()->getSnapshot();

    const auto transportSnapshot = sequencerSnapshot.getTransportStateView();

    if (!transportSnapshot.isTempoSourceSequenceEnabled())
    {
        return transportSnapshot.getMasterTempo();
    }

    const auto sequenceSnapshot =
        sequencerSnapshot.getSelectedSequenceStateView();

    if (!sequenceSnapshot.isUsed())
    {
        return DefaultTempo;
    }

    const auto tempoChangeEvent = sequencer.getCurrentTempoChangeEventData();

    if (transportSnapshot.isTempoSourceSequenceEnabled())
    {
        const auto ignoreTempoChangeScreen =
            sequencer.getScreens()->get<ScreenId::IgnoreTempoChangeScreen>();

        if (sequenceSnapshot.isTempoChangeEnabled() ||
            (sequencer.isSongModeEnabled() &&
             !ignoreTempoChangeScreen->getIgnore()))
        {
            if (tempoChangeEvent)
            {

                return tempoChangeEvent->getTempo(
                    sequenceSnapshot.getInitialTempo());
            }
        }

        return sequenceSnapshot.getInitialTempo();
    }

    if (sequenceSnapshot.isTempoChangeEnabled() && tempoChangeEvent)
    {
        return tempoChangeEvent->getTempo(transportSnapshot.getMasterTempo());
    }

    return transportSnapshot.getMasterTempo();
}
