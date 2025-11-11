#include "MidiClockOutput.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"

#include "engine/EngineHost.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"

#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"

#include <concurrentqueue.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

MidiClockOutput::MidiClockOutput(
    Sequencer *sequencer,
    const std::function<std::shared_ptr<Screens>()> &getScreens,
    const std::function<bool()> &isBouncing)
    : sequencer(sequencer), getScreens(getScreens), isBouncing(isBouncing)
// midiSyncStartStopContinueMsg(std::make_shared<ShortMessage>()),
// msg(std::make_shared<ShortMessage>())
{
    eventQueue = std::make_shared<
        moodycamel::ConcurrentQueue<engine::EventAfterNFrames>>(100);
    tempEventQueue.reserve(100);
    // msg->setMessage(ShortMessage::TIMING_CLOCK);
}

void MidiClockOutput::sendMidiClockMsg(int frameIndex) const
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

void MidiClockOutput::sendMidiSyncMsg(unsigned char status) const
{
    // midiSyncStartStopContinueMsg->setMessage(status);

    // bufferpos should be set by SequencerPlaybackEngine when it's actually
    // emitting these events, i.e. enqueueing them for host processing
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
    const double tempo = sequencer->getTransport()->getTempo();

    if (tempo != clock.getBpm())
    {
        clock.set_bpm(tempo);
    }
}

void MidiClockOutput::enqueueEventAfterNFrames(
    const std::function<void()> &event, const unsigned long nFrames) const
{
    engine::EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue->enqueue(std::move(e));
}

void MidiClockOutput::enqueueMidiSyncStart1msBeforeNextClock() const
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
            sendMidiSyncMsg(sequencer->getTransport()->getPlayStartPositionQuarterNotes()
    == 0.0 ? ShortMessage::START : ShortMessage::CONTINUE);
        },
        numberOfFramesBeforeMidiSyncStart);
        */
}

void MidiClockOutput::setSampleRate(const unsigned int sampleRate)
{
    requestedSampleRate = sampleRate;
}

void MidiClockOutput::processEventsAfterNFrames()
{
    engine::EventAfterNFrames batch[100];

    const size_t count = eventQueue->try_dequeue_bulk(batch, 100);

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
        eventQueue->enqueue(std::move(e));
    }
}

void MidiClockOutput::processFrame(bool isRunningAtStartOfBuffer,
                                   int frameIndex) const
{
    /*
    lastProcessedFrameIsMidiClockLock = false;

    if (isBouncing())
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
            // sendMidiSyncMsg(ShortMessage::STOP);
            wasRunning = false;
        }
    }
    */
}

void MidiClockOutput::processSampleRateChange()
{
    if (clock.getSampleRate() != requestedSampleRate)
    {
        const auto bpm = clock.getBpm();
        clock.init(requestedSampleRate);
        clock.set_bpm(bpm);
    }
}

bool MidiClockOutput::isLastProcessedFrameMidiClockLock() const
{
    return lastProcessedFrameIsMidiClockLock;
}
