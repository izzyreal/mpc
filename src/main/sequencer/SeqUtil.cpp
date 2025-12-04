#include "sequencer/SeqUtil.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "SequencerStateManager.hpp"

#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/TempoChangeEvent.hpp"
#include "sequencer/TimeSignature.hpp"

using namespace mpc::sequencer;

int SeqUtil::getTickFromBar(const int i, const Sequence *s, int position)
{
    if (i < 0)
    {
        return 0;
    }

    const auto difference = i - getBarFromTick(s, position);
    const auto den = s->getTimeSignatureFromTickPos(position).denominator;

    if (const auto denTicks = static_cast<int>(
            Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
        position + difference * denTicks * 4 > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + difference * denTicks * 4;
    }

    return position;
}

int SeqUtil::getBarFromTick(const Sequence *s, const int position)
{
    if (position == 0)
    {
        return 0;
    }

    const auto [num, den] = s->getTimeSignatureFromTickPos(position);
    const auto denTicks = static_cast<int>(
        Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
    const auto bar = static_cast<int>(floor(position / (denTicks * num)));

    return bar;
}

double SeqUtil::secondsPerTick(const double tempo)
{
    return 60.0 / tempo / Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ;
}

double SeqUtil::ticksPerSecond(const double tempo)
{

    const auto bps = tempo / 60.0;
    return bps * Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ;
}

double SeqUtil::ticksToFrames(const double ticks, const double tempo,
                              const int sampleRate)
{
    return ticks * secondsPerTick(tempo) * sampleRate;
}

double SeqUtil::framesToTicks(const double frames, const double tempo,
                              const int sampleRate)
{
    return frames / (secondsPerTick(tempo) * sampleRate);
}

double SeqUtil::sequenceFrameLength(const Sequence *seq, const int firstTick,
                                    const int lastTick, const int sr)
{
    double result = 0;
    auto lastTceTick = firstTick;
    const auto tempoChangeEvents = seq->getTempoChangeEvents();
    const auto tceSize = tempoChangeEvents.size();
    std::shared_ptr<TempoChangeEvent> lastTce;

    if (tceSize == 0)
    {
        result =
            ticksToFrames(lastTick - firstTick, seq->getInitialTempo(), sr);
        return result;
    }

    if (const auto firstTceTick = tempoChangeEvents[0]->getTick();
        firstTick < firstTceTick)
    {
        result =
            ticksToFrames(firstTceTick - firstTick, seq->getInitialTempo(), sr);
    }

    for (int i = 0; i < tceSize - 1; i++)
    {
        const auto nextTce = tempoChangeEvents[i + 1];

        if (firstTick > nextTce->getTick())
        {
            continue;
        }

        if (lastTick < nextTce->getTick())
        {
            lastTce = nextTce;
            break;
        }

        const auto tce = tempoChangeEvents[i];
        result += ticksToFrames(nextTce->getTick() - lastTceTick,
                                tce->getTempo(), sr);
        lastTceTick = nextTce->getTick();
    }

    if (!lastTce)
    {
        lastTce = tempoChangeEvents[0];
    }

    result +=
        ticksToFrames(lastTick - lastTce->getTick(), lastTce->getTempo(), sr);
    return static_cast<int>(ceil(result));
}

int SeqUtil::loopFrameLength(const Sequence *seq, const int sr)
{
    return static_cast<int>(sequenceFrameLength(seq, seq->getLoopStartTick(),
                                                seq->getLoopEndTick(), sr));
}

int SeqUtil::songFrameLength(Song *song, Sequencer *sequencer, const int sr)
{
    double result = 0;
    const auto steps = song->getStepCount();

    for (int i = 0; i < steps; i++)
    {
        for (int j = 0; j < song->getStep(i).lock()->getRepeats(); j++)
        {
            const auto seq =
                sequencer->getSequence(song->getStep(i).lock()->getSequence())
                    .get();
            result += sequenceFrameLength(seq, 0, seq->getLastTick(), sr);
        }
    }

    return static_cast<int>(result);
}

int SeqUtil::setBar(int i, const Sequence *sequence, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    const auto difference = i - getBar(sequence, position);
    const auto den =
        sequence->getTimeSignatureFromTickPos(position).denominator;

    if (const auto denTicks = static_cast<int>(
            Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
        position + difference * denTicks * 4 > sequence->getLastTick())
    {
        position = sequence->getLastTick();
    }
    else
    {
        position = position + difference * denTicks * 4;
    }

    return position;
}

int SeqUtil::setBeat(int i, const Sequence *s, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    const auto [num, den] = s->getTimeSignatureFromTickPos(position);

    if (i >= num)
    {
        i = num - 1;
    }

    const auto difference = i - getBeat(s, position);

    if (const auto denTicks = static_cast<int>(
            Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
        position + difference * denTicks > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + difference * denTicks;
    }

    return position;
}

int SeqUtil::setClock(int i, const Sequence *s, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    const auto den = s->getTimeSignatureFromTickPos(position).denominator;

    if (const auto denTicks = static_cast<int>(
            Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
        i >= denTicks)
    {
        i = denTicks - 1;
    }

    if (const auto difference = i - getClock(s, position);
        position + difference > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + difference;
    }

    return position;
}

int SeqUtil::getBar(const Sequence *s, const int position)
{
    if (position == 0)
    {
        return 0;
    }

    const auto [num, den] = s->getTimeSignatureFromTickPos(position);
    const auto denTicks = static_cast<int>(
        Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
    const auto bar = static_cast<int>(floor(position / (denTicks * num)));
    return bar;
}

int SeqUtil::getBeat(const Sequence *s, const int position)
{
    if (position == 0)
    {
        return 0;
    }

    const auto den = s->getTimeSignatureFromTickPos(position).denominator;
    const auto denTicks = static_cast<int>(
        Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
    auto beat = static_cast<int>(floor(position / denTicks));
    beat = beat % den;
    return beat;
}

int SeqUtil::getClock(const Sequence *s, const int position)
{
    const auto den = s->getTimeSignatureFromTickPos(position).denominator;
    const auto denTicks = static_cast<int>(
        Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));

    if (position == 0)
    {
        return 0;
    }

    const auto clock = position % denTicks;
    return clock;
}

void SeqUtil::copyBars(Mpc &mpc, const uint8_t fromSeqIndex,
                       const uint8_t toSeqIndex, const uint8_t copyFirstBar,
                       const uint8_t copyLastBar, const uint8_t copyCount,
                       const uint8_t copyAfterBar)
{
    const auto sequencer = mpc.getSequencer();

    const auto fromSequence = sequencer->getSequence(fromSeqIndex);

    if (!fromSequence->isUsed())
    {
        return;
    }

    const auto toSequence = sequencer->getSequence(toSeqIndex);

    const auto destinationBarCount = std::clamp(
        static_cast<uint16_t>((copyLastBar - copyFirstBar + 1) * copyCount),
        static_cast<uint16_t>(0), Mpc2000XlSpecs::MAX_BAR_COUNT);

    CopyBars copyBars{
        SequenceIndex(fromSeqIndex),
        SequenceIndex(toSeqIndex),
        BarIndex(copyFirstBar),
        BarIndex(copyLastBar),
        copyCount,
        BarIndex(copyAfterBar),
        destinationBarCount,
    };

    if (toSequence->isUsed())
    {
        if (toSequence->getLastBarIndex() + destinationBarCount >
            Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
        {
            copyBars.destinationBarCount = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX -
                                           toSequence->getLastBarIndex();
        }

        auto onComplete =
            [stateManager = sequencer->getStateManager(), copyBars]
        {
            stateManager->enqueue(copyBars);
        };

        toSequence->insertBars(destinationBarCount, BarIndex(copyAfterBar),
                               onComplete);
    }
    else
    {
        toSequence->init(destinationBarCount - 1);
        sequencer->getStateManager()->enqueue(copyBars);
    }
}

bool SeqUtil::isRecMainWithoutPlaying(
    const std::shared_ptr<Sequencer> &sequencer,
    const std::string &currentScreenName,
    const std::shared_ptr<controller::ClientHardwareEventController>
        &clientHardwareEventController)
{
    const bool posIsLastTick = sequencer->getTransport()->getTickPosition() ==
                               sequencer->getSelectedSequence()->getLastTick();

    const bool recIsPressedOrLocked =
        clientHardwareEventController->isRecLockedOrPressed();

    const bool recMainWithoutPlaying =
        currentScreenName == "sequencer" &&
        (!sequencer->getTransport()->isPlaying() ||
         sequencer->getTransport()->isMetronomeOnlyEnabled()) &&
        recIsPressedOrLocked && !posIsLastTick;

    return recMainWithoutPlaying;
}

bool SeqUtil::isStepRecording(const std::string &currentScreenName,
                              const std::shared_ptr<Sequencer> &sequencer)
{
    auto posIsLastTick = [&]
    {
        return sequencer->getTransport()->getTickPosition() ==
               sequencer->getSelectedSequence()->getLastTick();
    };
    return currentScreenName == "step-editor" && !posIsLastTick();
}
