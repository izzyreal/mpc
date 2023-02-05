#include "FrameSeq.hpp"

#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

FrameSeq::FrameSeq(mpc::Mpc& mpc) : FrameSeqBase(mpc)
{
}

void FrameSeq::work(int nFrames)
{
    const bool isRunningAtStartOfBuffer = running.load();

    processTempoChange();

    auto seq = sequencer->getCurrentlyPlayingSequence();

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        processEventsAfterNFrames(frameIndex);

        if (!clock.proc())
        {
            continue;
        }

        processSampleRateChange();

        tickFrameOffset = frameIndex;

        displayPunchRects();

        const bool sequencerShouldPlay = processTransport(isRunningAtStartOfBuffer, frameIndex);

        if (sequencerShouldPlay)
        {
            triggerClickIfNeeded();

            if (metronome)
            {
                sequencerPlayTickCounter++;
                continue;
            }

            if (sequencer->isCountingIn())
            {
                sequencerPlayTickCounter++;
                stopCountingInIfRequired();
                continue;
            }

            updateTimeDisplay();

            if (sequencerPlayTickCounter >= seq->getLastTick() - 1 &&
                !sequencer->isSongModeEnabled() &&
                sequencer->getNextSq() != -1)
            {
                seq = switchToNextSequence();
                continue;
            }
            else if (sequencer->isSongModeEnabled())
            {
                if (processSongMode())
                {
                    continue;
                }
            }
            else if (seq->isLoopEnabled())
            {
                if (processSeqLoopEnabled())
                {
                    continue;
                }
            }
            else
            {
                processSeqLoopDisabled();
            }

            sequencer->playToTick(static_cast<int>(sequencerPlayTickCounter));
            processNoteRepeat();

            sequencerPlayTickCounter++;
        }

        continuousTickCounter++;
    }
}
