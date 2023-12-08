#include "FrameSeq.hpp"

#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

FrameSeq::FrameSeq(mpc::Mpc& mpc) : FrameSeqBase(mpc)
{
}

void FrameSeq::work(int nFrames)
{
    const bool isRunningAtStartOfBuffer = running.load();

    auto seq = sequencer->getCurrentlyPlayingSequence();

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    processSampleRateChange();
    processTempoChange();

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        processEventsAfterNFrames(frameIndex);

        if (syncScreen->modeOut != 0)
        {
            if (clock.proc())
            {
                tickFrameOffset = frameIndex;
            }
            else
            {
                continue;
            }
        }

        const bool sequencerShouldPlay = processTransport(isRunningAtStartOfBuffer, frameIndex);

        if (sequencerShouldPlay)
        {
            if (syncScreen->modeOut == 0)
            {
                if (clock.proc())
                {
                    tickFrameOffset = frameIndex;
                }
                else
                {
                    continue;
                }
            }

            triggerClickIfNeeded();
            displayPunchRects();

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
                if (!songHasStopped && processSongMode())
                {
                    songHasStopped = true;
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
                if (!normalPlayHasStopped && processSeqLoopDisabled())
                {
                    normalPlayHasStopped = true;
                }
            }

            if (!songHasStopped && !normalPlayHasStopped)
            {
                sequencer->playToTick(static_cast<int>(sequencerPlayTickCounter));
                processNoteRepeat();
            }

            sequencerPlayTickCounter++;
        }
    }
}
