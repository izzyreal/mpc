#include "Transport.hpp"

#include "FloatTypes.hpp"
#include "SequencerStateManager.hpp"
#include "SequenceStateView.hpp"
#include "Sequencer.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/ScreenIdGroups.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/window/IgnoreTempoChangeScreen.hpp"
#include "sequencer/Sequence.hpp"
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
    return sequencer.getStateManager()
        ->getSnapshot()
        .getTransportStateView()
        .isSequencerRunning();
}

void Transport::play(const bool fromStart) const
{
    const bool isSongMode = sequencer.isSongModeEnabled();

    if (isSongMode)
    {
        const auto songScreen =
            sequencer.getScreens()->get<ScreenId::SongScreen>();

        const auto currentSong =
            sequencer.getSong(songScreen->getSelectedSongIndex());

        if (!currentSong->isUsed())
        {
            return;
        }

        if (songScreen->getOffset() + 1 >= currentSong->getStepCount() &&
            !fromStart)
        {
            return;
        }

        const int step = fromStart ? 0 : songScreen->getOffset() + 1;

        if (step < currentSong->getStepCount())
        {
            if (const std::shared_ptr<Step> currentStep =
                    currentSong->getStep(step).lock();
                !sequencer.getSequence(currentStep->getSequence())->isUsed())
            {
                return;
            }
        }
    }
    else
    {
        if (!sequencer.getSelectedSequence()->isUsed())
        {
            return;
        }
    }

    if (fromStart)
    {
        if (isSongMode)
        {
            sequencer.getStateManager()->enqueue(PlaySongFromStart{});
        }
        else
        {
            sequencer.getStateManager()->enqueue(PlaySequenceFromStart{});
        }
    }
    else
    {
        if (isSongMode)
        {
            sequencer.getStateManager()->enqueue(PlaySong{});
        }
        else
        {
            sequencer.getStateManager()->enqueue(PlaySequence{});
        }
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

mpc::PositionQuarterNotes Transport::getWrappedPositionInSong(
    const PositionQuarterNotes positionQuarterNotes) const
{
    PositionQuarterNotes result = NoPositionQuarterNotes;

    const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();
    const auto song = sequencer.getSong(songScreen->getSelectedSongIndex());
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

            result = finalPosQuarterNotes;
            break;
        }
    }

    return result;
}

void Transport::moveSongToStepThatContainsPosition(
    const PositionQuarterNotes positionQuarterNotes) const
{
    const auto songScreen = sequencer.getScreens()->get<ScreenId::SongScreen>();
    const auto song = sequencer.getSong(songScreen->getSelectedSongIndex());

    uint32_t stepStartTick = 0, stepEndTick = 0;

    for (uint8_t stepIndex = 0; stepIndex < song->getStepCount(); stepIndex++)
    {
        stepStartTick = stepEndTick;

        const auto step = song->getStep(stepIndex).lock();

        if (const auto sequence = sequencer.getSequence(step->getSequence());
            sequence->isUsed())
        {
            stepEndTick =
                stepStartTick + sequence->getLastTick() * step->getRepeats();
        }

        const auto stepStartPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepStartTick);
        const auto stepEndPositionQuarterNotes =
            Sequencer::ticksToQuarterNotes(stepEndTick);

        if (positionQuarterNotes >= stepStartPositionQuarterNotes &&
            positionQuarterNotes < stepEndPositionQuarterNotes)
        {
            songScreen->setOffset(stepIndex - 1);
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

    if (!seq) return 0;

    const auto pos = getTickPositionGuiPresentation();

    return seq->getBarIndexForPositionTicks(pos);
}

int Transport::getCurrentBeatIndex() const
{
    const auto seq = isPlaying() ? sequencer.getCurrentlyPlayingSequence()
                                 : sequencer.getSelectedSequence();

    if (!seq) return 0;

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

    const auto den = seq->getTimeSignature().denominator;
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

    if (!sequence) return 0;

    auto clock = getTickPositionGuiPresentation();

    if (clock == sequence->getLastTick())
    {
        return 0;
    }

    if (isPlaying() && !isCountingIn())
    {
        if (clock > sequence->getLastTick())
        {
            clock %= sequence->getLastTick();
        }
    }

    const auto den = sequence->getTimeSignature().denominator;
    const auto denTicks = 96 * (4.0 / den);

    if (clock == 0)
    {
        return 0;
    }

    const auto currentBarIndex = getCurrentBarIndex();

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; ++i)
    {
        if (i == currentBarIndex)
        {
            break;
        }

        clock -= sequence->getBarLength(i);
    }

    const auto currentBeatIndex = getCurrentBeatIndex();

    for (int i = 0; i < currentBeatIndex; i++)
    {
        clock -= denTicks;
    }

    return clock;
}

void Transport::setBarBeatClock(const int bar, const int beat,
                                const int clock) const
{
    if (isPlaying())
    {
        return;
    }

    const auto seq = sequencer.getSelectedSequence();

    const auto [num, den] = seq->getTimeSignature();

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

    i = std::clamp(i, 0, static_cast<int>(Mpc2000XlSpecs::MAX_LAST_BAR_INDEX));

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

    const auto [num, den] = s->getTimeSignature();

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

    const auto den = s->getTimeSignature().denominator;

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
    sequencer.getStateManager()->enqueue(
        SetPositionQuarterNotes{positionQuarterNotes});

    if (!isPlaying())
    {
        sequencer.getStateManager()->enqueue(SyncTrackEventIndices{});
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

    const auto s = sequencer.getSelectedSequence();
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
    if (!isPlaying() && !sequencer.getSelectedSequence()->isUsed())
    {
        return tempo;
    }

    const auto seq = sequencer.getSelectedSequence();

    if (screengroups::isSongScreen(
            sequencer.layeredScreen->getCurrentScreenId()))
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

        if (seq->isTempoChangeOn() || (sequencer.isSongModeEnabled() &&
                                       !ignoreTempoChangeScreen->getIgnore()))
        {
            if (tce)
            {
                return tce->getTempo();
            }
        }

        return sequencer.getSelectedSequence()->getInitialTempo();
    }

    if (seq->isTempoChangeOn() && tce)
    {
        return tempo * tce->getRatio() * 0.001;
    }

    return tempo;
}
