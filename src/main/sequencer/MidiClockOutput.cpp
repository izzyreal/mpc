#include "MidiClockOutput.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"

#include "audiomidi/MidiOutput.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SyncScreen.hpp"

#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"

#include <concurrentqueue.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;
using namespace mpc::client::event;

MidiClockOutput::MidiClockOutput(
const std::function<int()> &getSampleRate,
    const std::function<std::shared_ptr<audiomidi::MidiOutput>()>
        &getMidiOutput,
    Sequencer *sequencer,
    const std::function<std::shared_ptr<Screens>()> &getScreens,
    const std::function<bool()> &isBouncing)
    : getSampleRate(getSampleRate), getMidiOutput(getMidiOutput), sequencer(sequencer), getScreens(getScreens), isBouncing(isBouncing)
{
    eventQueue = std::make_shared<
        moodycamel::ConcurrentQueue<engine::EventAfterNFrames>>(100);
    tempEventQueue.reserve(100);
}

void MidiClockOutput::sendMidiClockMsg(const int frameIndex) const
{
    const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();

    if (syncScreen->getModeOut() > 0)
    {
        ClientMidiEvent e;
        e.setMessageType(ClientMidiEvent::MIDI_CLOCK);
        e.setBufferOffset(frameIndex);
        getMidiOutput()->enqueueEvent(e);
    }
}

void MidiClockOutput::sendMidiSyncMsg(
    const ClientMidiEvent::MessageType msgType, const int sampleNumber) const
{
    const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();

    if (syncScreen->getModeOut() > 0)
    {
        ClientMidiEvent e;
        e.setMessageType(msgType);
        e.setBufferOffset(sampleNumber);

        getMidiOutput()->enqueueEvent(e);
    }
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
    const std::function<void(int)> &event, const unsigned long nFrames) const
{
    engine::EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue->enqueue(std::move(e));
}

void MidiClockOutput::enqueueMidiSyncStart1msBeforeNextClock() const
{
    const auto durationToNextClockInFrames =
        static_cast<unsigned int>(SeqUtil::ticksToFrames(
            4, clock.getBpm(), static_cast<float>(clock.getSampleRate())));

    const auto oneMsInFrames =
        static_cast<unsigned int>(clock.getSampleRate() / 1000.f);

    const unsigned int numberOfFramesBeforeMidiSyncStart =
        durationToNextClockInFrames - oneMsInFrames;

    enqueueEventAfterNFrames(
        [&](const int sampleNumber)
        {
            const auto playStartPosition =
                sequencer->getTransport()->getPlayStartPositionQuarterNotes();
            const auto msgType = playStartPosition == 0.0
                                     ? ClientMidiEvent::MIDI_START
                                     : ClientMidiEvent::MIDI_CONTINUE;
            sendMidiSyncMsg(msgType, sampleNumber);
        },
        numberOfFramesBeforeMidiSyncStart);
}

void MidiClockOutput::processEventsAfterNFrames(const int sampleNumber)
{
    engine::EventAfterNFrames batch[100];

    const size_t count = eventQueue->try_dequeue_bulk(batch, 100);

    tempEventQueue.clear();

    for (size_t i = 0; i < count; ++i)
    {
        auto &e = batch[i];
        if (++e.frameCounter >= e.nFrames)
        {
            e.f(sampleNumber);
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

void MidiClockOutput::processFrame(const bool isRunningAtStartOfBuffer,
                                   const int frameIndex,
                                   int tickCountAtThisFrame)
{
    lastProcessedFrameIsMidiClockLock = false;

    if (isBouncing())
    {
        return;
    }

    processEventsAfterNFrames(frameIndex);

    if (!isRunningAtStartOfBuffer)
    {
        if (clock.proc())
        {
            tickCountAtThisFrame = 1;
        }
        else
        {
            return;
        }
    }

    if (isRunningAtStartOfBuffer && tickCountAtThisFrame == 0)
    {
        return;
    }

    const auto lockedToClock = midiClockTickCounter == 0;

    midiClockTickCounter += tickCountAtThisFrame;

    lastProcessedFrameIsMidiClockLock = true;

    if (midiClockTickCounter >= 4)
    {
        midiClockTickCounter = 0;
    }

    const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();

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
            sendMidiSyncMsg(ClientMidiEvent::MIDI_STOP, frameIndex);
            wasRunning = false;
        }
    }
}

void MidiClockOutput::processSampleRateChange()
{
    const auto sampleRate = getSampleRate();
    if (clock.getSampleRate() != sampleRate)
    {
        const auto bpm = clock.getBpm();
        clock.init(sampleRate);
        clock.set_bpm(bpm);
    }
}

bool MidiClockOutput::isLastProcessedFrameMidiClockLock() const
{
    return lastProcessedFrameIsMidiClockLock;
}
