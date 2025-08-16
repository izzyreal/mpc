#include "MidiClockOutput.hpp"

#include "Mpc.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"

#include "sequencer/SeqUtil.hpp"

#include <engine/midi/ShortMessage.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace mpc::engine::midi;

MidiClockOutput::MidiClockOutput(mpc::Mpc& mpc)
        : mpc(mpc),
          sequencer(mpc.getSequencer()),
          syncScreen(mpc.screens->get<SyncScreen>("sync")),
          midiSyncStartStopContinueMsg(std::make_shared<ShortMessage>()),
          msg(std::make_shared<ShortMessage>())
{
    msg->setMessage(ShortMessage::TIMING_CLOCK);
}

void MidiClockOutput::sendMidiClockMsg(int frameIndex)
{
    auto clockMsg = std::make_shared<mpc::engine::midi::ShortMessage>();
    clockMsg->setMessage(ShortMessage::TIMING_CLOCK);

    if (syncScreen->getModeOut() > 0) {
        clockMsg->bufferPos = frameIndex;

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueueMessageOutputA(clockMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueueMessageOutputB(clockMsg);
        }
    }
}

void MidiClockOutput::sendMidiSyncMsg(unsigned char status, unsigned int frameIndex)
{
    midiSyncStartStopContinueMsg->setMessage(status);

    midiSyncStartStopContinueMsg->bufferPos = static_cast<int>(frameIndex);

    if (syncScreen->getModeOut() > 0)
    {
        midiSyncStartStopContinueMsg->setMessage(status);

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputA(midiSyncStartStopContinueMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputB(midiSyncStartStopContinueMsg);
        }
    }
}

void MidiClockOutput::processTempoChange()
{
    double tempo = sequencer->getTempo();

    if (tempo != clock.getBpm())
    {
        clock.set_bpm(tempo);
    }
}

void MidiClockOutput::enqueueEventAfterNFrames(const std::function<void(unsigned int)>& event, unsigned long nFrames)
{
    for (auto &e : eventsAfterNFrames)
    {
        if (!e.occupied.load())
        {
            e.init(nFrames, event);
            break;
        }
    }
}

void MidiClockOutput::enqueueMidiSyncStart1msBeforeNextClock()
{
    const auto durationToNextClockInFrames =
            static_cast<unsigned int>(SeqUtil::ticksToFrames(4, clock.getBpm(), static_cast<float>(clock.getSampleRate())));

    const auto oneMsInFrames = static_cast<unsigned int>(clock.getSampleRate() / 1000.f);

    const unsigned int numberOfFramesBeforeMidiSyncStart = durationToNextClockInFrames - oneMsInFrames;

    enqueueEventAfterNFrames([&](unsigned int frameIndex){
        sendMidiSyncMsg(sequencer->getPlayStartPpqPosition() == 0.0 ? ShortMessage::START : ShortMessage::CONTINUE, frameIndex);
    }, numberOfFramesBeforeMidiSyncStart);
}

void MidiClockOutput::setSampleRate(unsigned int sampleRate)
{
    requestedSampleRate = sampleRate;
}

void MidiClockOutput::processEventsAfterNFrames(int frameIndex)
{
    for (auto& e : eventsAfterNFrames)
    {
        if (!e.occupied.load()) continue;

        e.frameCounter += 1;

        if (e.frameCounter >= e.nFrames)
        {
            e.f(frameIndex);
            e.reset();
        }
    }
}

void MidiClockOutput::processFrame(bool isRunningAtStartOfBuffer, int frameIndex)
{
    lastProcessedFrameIsMidiClockLock = false;

    if (mpc.getAudioMidiServices()->isBouncing())
    {
        return;
    }

    processEventsAfterNFrames(frameIndex);

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
            sendMidiSyncMsg(ShortMessage::STOP, frameIndex);
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
