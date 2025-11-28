#include "sequencer/SeqUtil.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"
#include "NonRtSequencerStateManager.hpp"

#include "controller/ClientHardwareEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
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
    const auto den = s->getTimeSignature().denominator;

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

    const auto [num, den] = s->getTimeSignature();
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
    return static_cast<int>(
        sequenceFrameLength(seq, seq->getLoopStartTick(), seq->getLoopEndTick(), sr));
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
    const auto den = sequence->getTimeSignature().denominator;

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

    const auto [num, den] = s->getTimeSignature();

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

    const auto den = s->getTimeSignature().denominator;

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

    const auto [num, den] = s->getTimeSignature();
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

    const auto den = s->getTimeSignature().denominator;
    const auto denTicks = static_cast<int>(
        Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ * (4.0 / den));
    auto beat = static_cast<int>(floor(position / denTicks));
    beat = beat % den;
    return beat;
}

int SeqUtil::getClock(const Sequence *s, const int position)
{
    const auto den = s->getTimeSignature().denominator;
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
    auto numberOfDestinationBars = (copyLastBar - copyFirstBar + 1) * copyCount;

    if (numberOfDestinationBars > Mpc2000XlSpecs::MAX_BAR_COUNT)
    {
        numberOfDestinationBars = Mpc2000XlSpecs::MAX_BAR_COUNT;
    }

    if (!toSequence->isUsed())
    {
        toSequence->init(numberOfDestinationBars - 1);
    }
    else
    {
        if (toSequence->getLastBarIndex() + numberOfDestinationBars >
            Mpc2000XlSpecs::MAX_LAST_BAR_INDEX)
        {
            numberOfDestinationBars = Mpc2000XlSpecs::MAX_LAST_BAR_INDEX -
                                      toSequence->getLastBarIndex();
        }

        toSequence->insertBars(numberOfDestinationBars, BarIndex(copyAfterBar));
    }

    int sourceBarCounter = 0;

    const auto numberOfSourceBars = copyLastBar + 1 - copyFirstBar;

    for (int i = 0; i < numberOfDestinationBars; i++)
    {
        toSequence->setTimeSignature(
            i + copyAfterBar,
            fromSequence->getNumerator(sourceBarCounter + copyFirstBar),
            fromSequence->getDenominator(sourceBarCounter + copyFirstBar));

        sourceBarCounter++;

        if (sourceBarCounter >= numberOfSourceBars)
        {
            sourceBarCounter = 0;
        }
    }

    auto firstTickOfFromSequence = 0;
    auto lastTickOfFromSequence = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        if (i == copyFirstBar)
        {
            break;
        }

        firstTickOfFromSequence += fromSequence->getBarLength(i);
    }

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        lastTickOfFromSequence += fromSequence->getBarLength(i);

        if (i == copyLastBar)
        {
            break;
        }
    }

    auto firstTickOfToSequence = 0;

    for (int i = 0; i < Mpc2000XlSpecs::MAX_BAR_COUNT; i++)
    {
        if (i == copyAfterBar)
        {
            break;
        }

        firstTickOfToSequence += toSequence->getBarLength(i);
    }

    const auto segmentLengthTicks =
        lastTickOfFromSequence - firstTickOfFromSequence;
    const auto offset = firstTickOfToSequence - firstTickOfFromSequence;

    for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; i++)
    {
        const auto t1 = fromSequence->getTrack(i);

        if (!t1->isUsed())
        {
            continue;
        }

        auto t1Events =
            t1->getEventRange(firstTickOfFromSequence, lastTickOfFromSequence);
        const auto t2 = toSequence->getTrack(i);

        sequencer->getNonRtStateManager()->drainQueue();

        if (!t2->isUsed())
        {
            t2->setUsed(true);
        }

        const auto toSeqLastTick = toSequence->getLastTick();

        for (const auto &event : t1Events)
        {
            const auto firstCopyTick = event->getTick() + offset;

            if (firstCopyTick >= toSeqLastTick)
            {
                // The events in Track are ordered by tick, so this and
                // any following event copies would be out of bounds.
                break;
            }

            for (auto k = 0; k < copyCount; k++)
            {
                const auto tick = firstCopyTick + k * segmentLengthTicks;

                // We do a more specific exit-check here, since within-bounds
                // copies may be followed by out-of-bounds ones.
                if (tick >= toSeqLastTick)
                {
                    break;
                }

                EventState eventToInsert = event->getSnapshot();

                eventToInsert.tick = tick;
                t2->insertEvent(eventToInsert, true);
            }
        }
    }
}

bool SeqUtil::isRecMainWithoutPlaying(
    const std::shared_ptr<Sequencer> &sequencer,
    const std::shared_ptr<TimingCorrectScreen> &timingCorrectScreen,
    const std::string &currentScreenName,
    const std::shared_ptr<hardware::Button> &recButton,
    const std::shared_ptr<controller::ClientHardwareEventController>
        &clientHardwareEventController)
{
    const auto tc_note = timingCorrectScreen->getNoteValue();
    const bool posIsLastTick = sequencer->getTransport()->getTickPosition() ==
                               sequencer->getSelectedSequence()->getLastTick();

    const bool recIsPressedOrLocked =
        recButton->isPressed() ||
        clientHardwareEventController->buttonLockTracker.isLocked(
            hardware::ComponentId::REC);

    const bool recMainWithoutPlaying =
        currentScreenName == "sequencer" &&
        !sequencer->getTransport()->isPlaying() && recIsPressedOrLocked &&
        tc_note != 0 && !posIsLastTick;

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

int SeqUtil::getEventTimeInSamples(
    const Sequence* seq,
    const int eventTick,
    const int currentTimeSamples,
    const SampleRate sampleRate)
{
    // 1. Loop length in samples
    const int loopLen = static_cast<int>(
        sequenceFrameLength(seq, 0, seq->getLastTick(), sampleRate));

    if (loopLen <= 0)
        return currentTimeSamples; // degenerate sequence

    // 2. Where are we inside the loop?
    const int phase = currentTimeSamples % loopLen;

    // 3. Convert event tick → sample offset from start of loop
    const int eventSample = static_cast<int>(
        sequenceFrameLength(seq, 0, eventTick, sampleRate));

    // 4. If event is still ahead in this loop: simple forward offset
    if (eventSample >= phase)
        return currentTimeSamples + (eventSample - phase);

    // 5. Otherwise event lies in the next loop iteration
    return currentTimeSamples + (loopLen - (phase - eventSample));
}

int SeqUtil::getTickCountForFrames(const Sequence* seq, const int firstTick,
                          const int frameCount, const int sr)
{
    const auto& tces = seq->getTempoChangeEvents();
    const int n = tces.size();

    auto secondsPerTick = [&](const double tempo) {
        return 60.0 / tempo / Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ;
    };

    int remainingFrames = frameCount;
    int tick = firstTick;

    double tempo = seq->getInitialTempo();

    // Find starting tempo segment.
    int i = 0;
    while (i < n && tces[i]->getTick() <= firstTick)
    {
        tempo = tces[i]->getTempo();
        i++;
    }

    // Process each tempo segment.
    while (i < n && remainingFrames > 0)
    {
        const int segmentEndTick = tces[i]->getTick();
        const int ticksAvailable = segmentEndTick - tick;

        if (ticksAvailable > 0)
        {
            const double secsPerTick = secondsPerTick(tempo);
            const double framesPerTick = secsPerTick * sr;

            // Frames needed to traverse full segment.
            const int framesForSegment = static_cast<int>(
                ceil(ticksAvailable * framesPerTick));

            if (framesForSegment > remainingFrames)
            {
                // We only advance partway through this segment.
                const double ticksInSegment =
                    remainingFrames / framesPerTick;

                return static_cast<int>(floor(ticksInSegment));
            }

            // Consume whole segment.
            remainingFrames -= framesForSegment;
            tick = segmentEndTick;
        }

        tempo = tces[i]->getTempo();
        i++;
    }

    // Final infinite segment (after last TCE).
    if (remainingFrames > 0)
    {
        const double secsPerTick = secondsPerTick(tempo);
        const double framesPerTick = secsPerTick * sr;

        const double ticksHere = remainingFrames / framesPerTick;
        return static_cast<int>(floor(ticksHere));
    }

    return 0;
}
