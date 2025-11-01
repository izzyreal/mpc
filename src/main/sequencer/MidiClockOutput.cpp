#include "MidiClockOutput.hpp"

#include "Mpc.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"

#include "sequencer/SeqUtil.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

MidiClockOutput::MidiClockOutput(mpc::Mpc &mpc)
    : mpc(mpc), sequencer(mpc.getSequencer()),
      syncScreen(mpc.screens->get<SyncScreen>())
      //midiSyncStartStopContinueMsg(std::make_shared<ShortMessage>()),
      //msg(std::make_shared<ShortMessage>())
{
    tempEventQueue.reserve(100);
    //msg->setMessage(ShortMessage::TIMING_CLOCK);
}

void MidiClockOutput::sendMidiClockMsg(int frameIndex)
{
    /*
    auto clockMsg = std::make_shared<mpc::engine::midi::ShortMessage>();
    clockMsg->setMessage(ShortMessage::TIMING_CLOCK);

    if (syncScreen->getModeOut() > 0)
    {
        clockMsg->bufferPos = frameIndex;

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputA(clockMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputB(clockMsg);
        }
    }
    */
}

void MidiClockOutput::sendMidiSyncMsg(unsigned char status)
{
    //midiSyncStartStopContinueMsg->setMessage(status);

    // bufferpos should be set by FrameSeq when it's actually emitting these
    // events, i.e. enqueueing them for host processing
    // midiSyncStartStopContinueMsg->bufferPos = static_cast<int>(frameIndex);
    /*

    if (syncScreen->getModeOut() > 0)
    {
        midiSyncStartStopContinueMsg->setMessage(status);

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputA(
                midiSyncStartStopContinueMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputB(
                midiSyncStartStopContinueMsg);
        }
    }
    */
}

void MidiClockOutput::processTempoChange()
{
    double tempo = sequencer->getTempo();

    if (tempo != clock.getBpm())
    {
        clock.set_bpm(tempo);
    }
}

void MidiClockOutput::enqueueEventAfterNFrames(
    const std::function<void()> &event, unsigned long nFrames)
{
    EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue.enqueue(std::move(e));
}

void MidiClockOutput::enqueueMidiSyncStart1msBeforeNextClock()
{
    /*
    const auto durationToNextClockInFrames =
        static_cast<unsigned int>(SeqUtil::ticksToFrames(
            4, clock.getBpm(), static_cast<float>(clock.getSampleRate())));

    const auto oneMsInFrames =
        static_cast<unsigned int>(clock.getSampleRate() / 1000.f);

    const unsigned int numberOfFramesBeforeMidiSyncStart =
        durationToNextClockInFrames - oneMsInFrames;

    enqueueEventAfterNFrames(
        [&]
        {
            sendMidiSyncMsg(sequencer->getPlayStartPositionQuarterNotes() == 0.0
                                ? ShortMessage::START
                                : ShortMessage::CONTINUE);
        },
        numberOfFramesBeforeMidiSyncStart);
        */
}

void MidiClockOutput::setSampleRate(unsigned int sampleRate)
{
    requestedSampleRate = sampleRate;
}

void MidiClockOutput::processEventsAfterNFrames()
{
    EventAfterNFrames batch[100];

    size_t count = eventQueue.try_dequeue_bulk(batch, 100);

    tempEventQueue.clear();

    for (size_t i = 0; i < count; ++i)
    {
        auto &e = batch[i];
        if (++e.frameCounter >= e.nFrames)
        {
            e.f();
        }
        else
        {
            tempEventQueue.push_back(std::move(e));
        }
    }

    for (auto &e : tempEventQueue)
    {
        eventQueue.enqueue(std::move(e));
    }
}

void MidiClockOutput::processFrame(bool isRunningAtStartOfBuffer,
                                   int frameIndex)
{
    lastProcessedFrameIsMidiClockLock = false;

    if (mpc.getAudioMidiServices()->isBouncing())
    {
        return;
    }

    processEventsAfterNFrames();

    if (!clock.proc())
    {
        return;
    }

    const auto lockedToClock = midiClockTickCounter++ == 0;

    lastProcessedFrameIsMidiClockLock = true;

    if (midiClockTickCounter == 4)
    {
        midiClockTickCounter = 0;
    }

    if (syncScreen->modeOut > 0)
    {
        if (lockedToClock)
        {
            sendMidiClockMsg(frameIndex);

            if (!wasRunning && isRunningAtStartOfBuffer)
            {
                wasRunning = true;
                enqueueMidiSyncStart1msBeforeNextClock();
            }
        }

        if (wasRunning && !isRunningAtStartOfBuffer)
        {
            //sendMidiSyncMsg(ShortMessage::STOP);
            wasRunning = false;
        }
    }
}

void MidiClockOutput::processSampleRateChange()
{
    if (clock.getSampleRate() != requestedSampleRate)
    {
        auto bpm = clock.getBpm();
        clock.init(requestedSampleRate);
        clock.set_bpm(bpm);
    }
}

bool MidiClockOutput::isLastProcessedFrameMidiClockLock()
{
    return lastProcessedFrameIsMidiClockLock;
}
