#include "sequencer/SeqUtil.hpp"

#include "Mpc.hpp"

#include "controller/ClientEventController.hpp"
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

#include <cmath>

using namespace mpc::sequencer;

int SeqUtil::getTickFromBar(int i, Sequence *s, int position)
{
    if (i < 0)
    {
        return 0;
    }

    auto difference = i - getBarFromTick(s, position);
    auto den = s->getTimeSignature().getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));

    if (position + (difference * denTicks * 4) > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + (difference * denTicks * 4);
    }

    return position;
}

int SeqUtil::getBarFromTick(Sequence *s, int position)
{
    if (position == 0)
    {
        return 0;
    }

    auto ts = s->getTimeSignature();
    auto num = ts.getNumerator();
    auto den = ts.getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));
    auto bar = (int)(floor(position / (denTicks * num)));

    return bar;
}

double SeqUtil::secondsPerTick(const double tempo)
{
    return 60.0 / tempo / 96.0;
}

double SeqUtil::ticksPerSecond(const double tempo)
{

    auto bps = tempo / 60.0;
    return bps * 96.0;
}

double SeqUtil::ticksToFrames(double ticks, const double tempo, int sr)
{
    return (ticks * secondsPerTick(tempo) * sr);
}

double SeqUtil::sequenceFrameLength(Sequence *seq, int firstTick, int lastTick, int sr)
{
    double result = 0;
    auto lastTceTick = firstTick;
    auto tempoChangeEvents = seq->getTempoChangeEvents();
    auto tceSize = tempoChangeEvents.size();
    std::shared_ptr<TempoChangeEvent> lastTce;

    if (tceSize == 0)
    {
        result = ticksToFrames(lastTick - firstTick, seq->getInitialTempo(), sr);
        return result;
    }
    else
    {
        auto firstTceTick = tempoChangeEvents[0]->getTick();

        if (firstTick < firstTceTick)
        {
            result = ticksToFrames(firstTceTick - firstTick, seq->getInitialTempo(), sr);
        }
    }

    for (int i = 0; i < tceSize - 1; i++)
    {
        auto nextTce = tempoChangeEvents[i + 1];

        if (firstTick > nextTce->getTick())
        {
            continue;
        }

        if (lastTick < nextTce->getTick())
        {
            lastTce = nextTce;
            break;
        }

        auto tce = tempoChangeEvents[i];
        result += ticksToFrames(nextTce->getTick() - lastTceTick, tce->getTempo(), sr);
        lastTceTick = nextTce->getTick();
    }

    if (!lastTce)
    {
        lastTce = tempoChangeEvents[0];
    }

    result += ticksToFrames(lastTick - lastTce->getTick(), lastTce->getTempo(), sr);
    return (int)(ceil(result));
}

int SeqUtil::loopFrameLength(Sequence *seq, int sr)
{
    return static_cast<int>(sequenceFrameLength(seq, seq->getLoopStart(), seq->getLoopEnd(), sr));
}

int SeqUtil::songFrameLength(Song *song, Sequencer *sequencer, int sr)
{
    double result = 0;
    auto steps = song->getStepCount();

    for (int i = 0; i < steps; i++)
    {
        for (int j = 0; j < song->getStep(i).lock()->getRepeats(); j++)
        {
            auto seq = sequencer->getSequence(song->getStep(i).lock()->getSequence()).get();
            result += sequenceFrameLength(seq, 0, seq->getLastTick(), sr);
        }
    }

    return static_cast<int>(result);
}
void SeqUtil::setTimeSignature(Sequence *sequence, int firstBarIndex, int tsLastBarIndex, int num, int den)
{
    for (int i = firstBarIndex; i <= tsLastBarIndex; i++)
    {
        setTimeSignature(sequence, i, num, den);
    }
}

void SeqUtil::setTimeSignature(Sequence *sequence, int bar, int num, int den)
{
    const auto newDenTicks = 96 * (4.0 / den);

    const auto barStart = sequence->getFirstTickOfBar(bar);
    const auto oldBarLength = sequence->getBarLengthsInTicks()[bar];
    const auto newBarLength = newDenTicks * num;
    const auto tickShift = newBarLength - oldBarLength;

    if (newBarLength < oldBarLength)
    {
        // The bar will be cropped, so we may have to remove some events
        // if they fall outside the new new bar's region.
        for (int tick = barStart + newBarLength; tick < barStart + oldBarLength; tick++)
        {
            for (auto &t : sequence->getTracks())
            {
                for (int eventIndex = t->getEvents().size() - 1; eventIndex >= 0; eventIndex--)
                {
                    if (t->getEvent(eventIndex)->getTick() == tick)
                    {
                        t->removeEvent(eventIndex);
                    }
                }
            }
        }
    }

    if (bar < 998)
    {
        // We're changing the timesignature of not the last bar, so
        // all bars after the bar we're changing are shifting. Here we
        // shift all relevant event ticks.
        const auto nextBarStartTick = sequence->getFirstTickOfBar(bar + 1);

        for (auto &t : sequence->getTracks())
        {
            for (int eventIndex = t->getEvents().size() - 1; eventIndex >= 0; eventIndex--)
            {
                auto event = t->getEvent(eventIndex);

                if (event->getTick() >= nextBarStartTick && event->getTick() < sequence->getLastTick())
                {
                    event->setTick(event->getTick() + tickShift);
                }
            }
        }
    }

    sequence->getBarLengthsInTicks()[bar] = newBarLength;
    sequence->getNumerators()[bar] = num;
    sequence->getDenominators()[bar] = den;
}

int SeqUtil::setBar(int i, Sequence *sequence, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    auto difference = i - SeqUtil::getBar(sequence, position);
    auto den = sequence->getTimeSignature().getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));

    if (position + (difference * denTicks * 4) > sequence->getLastTick())
    {
        position = sequence->getLastTick();
    }
    else
    {
        position = position + (difference * denTicks * 4);
    }

    return position;
}

int SeqUtil::setBeat(int i, Sequence *s, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    auto ts = s->getTimeSignature();
    auto num = ts.getNumerator();

    if (i >= num)
    {
        i = num - 1;
    }

    auto difference = i - SeqUtil::getBeat(s, position);

    auto den = ts.getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));

    if (position + (difference * denTicks) > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + (difference * denTicks);
    }

    return position;
}

int SeqUtil::setClock(int i, Sequence *s, int position)
{
    if (i < 0)
    {
        i = 0;
    }

    auto den = s->getTimeSignature().getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));

    if (i >= denTicks)
    {
        i = denTicks - 1;
    }

    auto difference = i - getClock(s, position);

    if (position + difference > s->getLastTick())
    {
        position = s->getLastTick();
    }
    else
    {
        position = position + difference;
    }

    return position;
}

int SeqUtil::getBar(Sequence *s, int position)
{
    if (position == 0)
    {
        return 0;
    }

    auto ts = s->getTimeSignature();
    auto num = ts.getNumerator();
    auto den = ts.getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));
    auto bar = (int)(floor(position / (denTicks * num)));
    return bar;
}

int SeqUtil::getBeat(Sequence *s, int position)
{
    if (position == 0)
    {
        return 0;
    }

    auto den = s->getTimeSignature().getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));
    auto beat = (int)(floor(position / (denTicks)));
    beat = beat % den;
    return beat;
}

int SeqUtil::getClock(Sequence *s, int position)
{
    auto den = s->getTimeSignature().getDenominator();
    auto denTicks = (int)(96 * (4.0 / den));

    if (position == 0)
    {
        return 0;
    }

    auto clock = (int)(position % (denTicks));
    return clock;
}

void SeqUtil::copyBars(mpc::Mpc &mpc, uint8_t fromSeqIndex, uint8_t toSeqIndex, uint8_t copyFirstBar, uint8_t copyLastBar, uint8_t copyCount, uint8_t copyAfterBar)
{
    const auto sequencer = mpc.getSequencer();

    auto fromSequence = sequencer->getSequence(fromSeqIndex);

    if (!fromSequence->isUsed())
    {
        return;
    }

    auto toSequence = sequencer->getSequence(toSeqIndex);
    auto numberOfDestinationBars = (copyLastBar - copyFirstBar + 1) * copyCount;

    if (numberOfDestinationBars > 999)
    {
        numberOfDestinationBars = 999;
    }

    if (!toSequence->isUsed())
    {
        toSequence->init(numberOfDestinationBars - 1);
    }
    else
    {
        if (toSequence->getLastBarIndex() + numberOfDestinationBars > 998)
        {
            numberOfDestinationBars = 998 - toSequence->getLastBarIndex();
        }

        toSequence->insertBars(numberOfDestinationBars, copyAfterBar);
    }

    int sourceBarCounter = 0;

    const auto numberOfSourceBars = (copyLastBar + 1) - copyFirstBar;

    for (int i = 0; i < numberOfDestinationBars; i++)
    {
        toSequence->setTimeSignature(i + copyAfterBar, fromSequence->getNumerator(sourceBarCounter + copyFirstBar), fromSequence->getDenominator(sourceBarCounter + copyFirstBar));

        sourceBarCounter++;

        if (sourceBarCounter >= numberOfSourceBars)
        {
            sourceBarCounter = 0;
        }
    }

    auto firstTickOfFromSequence = 0;
    auto lastTickOfFromSequence = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i == copyFirstBar)
        {
            break;
        }

        firstTickOfFromSequence += fromSequence->getBarLengthsInTicks()[i];
    }

    for (int i = 0; i < 999; i++)
    {
        lastTickOfFromSequence += fromSequence->getBarLengthsInTicks()[i];

        if (i == copyLastBar)
        {
            break;
        }
    }

    auto firstTickOfToSequence = 0;

    for (int i = 0; i < 999; i++)
    {
        if (i == copyAfterBar)
        {
            break;
        }

        firstTickOfToSequence += toSequence->getBarLengthsInTicks()[i];
    }

    const auto segmentLengthTicks = lastTickOfFromSequence - firstTickOfFromSequence;
    const auto offset = firstTickOfToSequence - firstTickOfFromSequence;

    for (int i = 0; i < 64; i++)
    {
        auto t1 = fromSequence->getTrack(i);

        if (!t1->isUsed())
        {
            continue;
        }

        auto t1Events = t1->getEventRange(firstTickOfFromSequence, lastTickOfFromSequence);
        auto t2 = toSequence->getTrack(i);

        if (!t2->isUsed())
        {
            t2->setUsed(true);
        }

        auto toSeqLastTick = toSequence->getLastTick();

        for (auto &event : t1Events)
        {
            auto firstCopyTick = event->getTick() + offset;

            if (firstCopyTick >= toSeqLastTick)
            {
                // The events in Track are ordered by tick, so this and
                // any following event copies would be out of bounds.
                break;
            }

            for (auto k = 0; k < copyCount; k++)
            {
                auto tick = firstCopyTick + (k * segmentLengthTicks);

                // We do a more specific exit-check here, since within-bounds
                // copies may be followed by out-of-bounds ones.
                if (tick >= toSeqLastTick)
                {
                    break;
                }

                t2->cloneEventIntoTrack(event, tick);
            }
        }
    }
}

bool SeqUtil::isRecMainWithoutPlaying(
    std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen> timingCorrectScreen,
    const std::string& currentScreenName,
    std::shared_ptr<mpc::hardware::Button> recButton,
    std::shared_ptr<mpc::controller::ClientHardwareEventController> clientHardwareEventController)
{
    auto tc_note = timingCorrectScreen->getNoteValue();
    bool posIsLastTick = sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();

    const bool recIsPressedOrLocked = recButton->isPressed() || 
                                      clientHardwareEventController->buttonLockTracker.isLocked(hardware::ComponentId::REC);

    bool recMainWithoutPlaying = currentScreenName == "sequencer" &&
                                 !sequencer->isPlaying() &&
                                 recIsPressedOrLocked &&
                                 tc_note != 0 &&
                                 !posIsLastTick;

    return recMainWithoutPlaying;
}

bool SeqUtil::isStepRecording(const std::string &currentScreenName, std::shared_ptr<Sequencer> sequencer)
{
    auto posIsLastTick = [&]
    {
        return sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();
    };
    return currentScreenName == "step-editor" && !posIsLastTick();
}
