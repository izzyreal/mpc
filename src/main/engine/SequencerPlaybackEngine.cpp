#include "engine/SequencerPlaybackEngine.hpp"

#include "audiomidi/EventHandler.hpp"
#include "sequencer/Transport.hpp"

#include "engine/NoteRepeatProcessor.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Clock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"

#include "sequencer/MidiClockOutput.hpp"
#include "sequencer/NonRtSequencerStateManager.hpp"
#include "sequencer/SeqUtil.hpp"

#include <concurrentqueue.h>

using namespace mpc::engine;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

SequencerPlaybackEngine::SequencerPlaybackEngine(
    Sequencer *sequencer, const std::shared_ptr<Clock> &clock,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    std::function<bool()> isBouncing, const std::function<int()> &getSampleRate,
    const std::function<bool()> &isRecMainWithoutPlaying,
    const std::function<void(int velo, int frameOffset)> &playMetronome,
    std::function<std::shared_ptr<Screens>()> getScreens,
    const std::function<bool()> &isNoteRepeatLockedOrPressed,
    const std::shared_ptr<NoteRepeatProcessor> &noteRepeatProcessor,
    const std::function<bool()> &isAudioServerCurrentlyRunningOffline)
    : getSampleRate(getSampleRate), layeredScreen(layeredScreen),
      getScreens(getScreens), sequencer(sequencer), clock(clock),
      isBouncing(isBouncing), isRecMainWithoutPlaying(isRecMainWithoutPlaying),
      playMetronome(playMetronome),
      isNoteRepeatLockedOrPressed(isNoteRepeatLockedOrPressed),
      noteRepeatProcessor(noteRepeatProcessor),
      isAudioServerCurrentlyRunningOffline(
          isAudioServerCurrentlyRunningOffline),
      midiClockOutput(
          std::make_shared<MidiClockOutput>(sequencer, getScreens, isBouncing))
{
    eventQueue = std::make_shared<EventQueue>(100);
    tempEventQueue.reserve(100);
}

void SequencerPlaybackEngine::start(const bool metronomeOnlyToUse)
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    if (getScreens()->get<ScreenId::SyncScreen>()->modeOut != 0)
    {
        // TODO Implement MIDI clock out
        // shouldWaitForMidiClockLock = true;
        shouldWaitForMidiClockLock = false;
    }

    clock->reset();

    metronomeOnly = metronomeOnlyToUse;
    metronomeOnlyTickPosition = 0;

    sequencerIsRunning.store(true);
}

void SequencerPlaybackEngine::startMetronome()
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    start(true);
}

void SequencerPlaybackEngine::stop()
{
    if (!sequencerIsRunning.load())
    {
        return;
    }

    sequencerIsRunning.store(false);
}

bool SequencerPlaybackEngine::isRunning() const
{
    return sequencerIsRunning.load();
}

void SequencerPlaybackEngine::enqueueEventAfterNFrames(
    const std::function<void()> &event, const unsigned long nFrames) const
{
    EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue->enqueue(std::move(e));
}

void SequencerPlaybackEngine::processEventsAfterNFrames()
{
    EventAfterNFrames batch[100];

    const size_t count = eventQueue->try_dequeue_bulk(batch, 100);

    tempEventQueue.clear();

    for (size_t i = 0; i < count; ++i)
    {
        if (auto &e = batch[i]; ++e.frameCounter >= e.nFrames)
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

void SequencerPlaybackEngine::work(const int nFrames)
{
    sequencer->getStateManager()->drainQueue();

    const auto playbackState =
        sequencer->getNonRtStateManager()->getSnapshot().getPlaybackState();
    const bool sequencerIsRunningAtStartOfBuffer = sequencerIsRunning.load();
    const auto currentTimeInSamplesAtStartOfBuffer = sequencer->getStateManager()->getSnapshot().getTimeInSamples();

    for (int i = 0; i < nFrames; ++i)
    {
        processEventsAfterNFrames();
    }

    if (!sequencerIsRunningAtStartOfBuffer)
    {
        sequencer->getStateManager()->enqueue(SetTimeInSamples{CurrentTimeInSamples});
        sequencer->getStateManager()->drainQueue();
        return;
    }

    sequencer->getStateManager()->enqueue(SetTimeInSamples{currentTimeInSamplesAtStartOfBuffer +
                               nFrames});
    sequencer->getStateManager()->drainQueue();

    const auto seq = sequencer->getCurrentlyPlayingSequence();

    for (auto &e : playbackState.events)
    {
        if (e.timeInSamples >= currentTimeInSamplesAtStartOfBuffer &&
            e.timeInSamples < currentTimeInSamplesAtStartOfBuffer + nFrames)
        {
            const auto frameOffsetInBuffer = e.timeInSamples - currentTimeInSamplesAtStartOfBuffer;

            if (e.eventState.type == EventType::MetronomeClick)
            {
                playMetronome(e.eventState.velocity, frameOffsetInBuffer);
                continue;
            }

            sequencer->getEventHandler()->handleFinalizedEvent(
                e.eventState, seq->getTrack(e.eventState.trackIndex).get(),
                frameOffsetInBuffer);
        }
    }

    const double tempo = sequencer->getTransport()->getTempo();
    const auto sampleRate = getSampleRate();
    const double tickCountForThisBuffer = SeqUtil::framesToTicks(nFrames, tempo, sampleRate);
    const double quarterNoteCountForThisBuffer = Sequencer::ticksToQuarterNotes(tickCountForThisBuffer);

    const auto wrappedPosition = sequencer->getTransport()->getWrappedPositionInSequence(sequencer->getTransport()->getPositionQuarterNotes() + quarterNoteCountForThisBuffer);
    sequencer->getTransport()->setPosition(wrappedPosition);
    sequencer->getStateManager()->drainQueue();
}

uint64_t SequencerPlaybackEngine::getMetronomeOnlyTickPosition() const
{
    return metronomeOnlyTickPosition;
}
