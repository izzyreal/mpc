#include "sequencer/NonRtSequencerStateWorker.hpp"

#include "Event.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "NonRtSequencerStateManager.hpp"
#include "Transport.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

NonRtSequencerStateWorker::NonRtSequencerStateWorker(Sequencer *sequencer)
    : running(false), sequencer(sequencer)
{
}

NonRtSequencerStateWorker::~NonRtSequencerStateWorker()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void NonRtSequencerStateWorker::start()
{
    if (running.load())
    {
        return;
    }

    running.store(true);

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    workerThread = std::thread(
        [&]
        {
            while (running.load())
            {
                work();
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        });
}

void NonRtSequencerStateWorker::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
}

void NonRtSequencerStateWorker::work()
{
    const auto currentTimeInSamples = sequencer->getSequencerPlaybackEngine()->getCurrentTimeInSamples();

    if (lastRenderedTimeInSamples != currentTimeInSamples)
    {
        lastRenderedTimeInSamples = currentTimeInSamples;
        if (currentTimeInSamples >= 0)
        {
            const auto playbackEngine = sequencer->getSequencerPlaybackEngine();
            const auto sampleRate = playbackEngine->getSampleRate();
            const auto playbackState = renderPlaybackState(SampleRate(sampleRate), currentTimeInSamples);
            sequencer->getNonRtStateManager()->enqueue(UpdatePlaybackState{std::move(playbackState)});
        }
        else
        {
            sequencer->getNonRtStateManager()->enqueue(UpdatePlaybackState{PlaybackState()});
        }
    }

    sequencer->getNonRtStateManager()->drainQueue();
}

PlaybackState NonRtSequencerStateWorker::renderPlaybackState(const SampleRate sampleRate, const TimeInSamples timeInSamples) const
{
    constexpr TimeInSamples snapshotWindowSizeSamples{30'000};

    PlaybackState result;
    result.timeInSamples = timeInSamples;

    const auto seqIndex = sequencer->isSongModeEnabled() ? sequencer->getSongSequenceIndex()
    : sequencer->getSelectedSequenceIndex();
    const auto seq = sequencer->getSequence(seqIndex);

    for (const auto &track : seq->getTracks())
    {
        for (const auto &event : track->getEvents())
        {
            const auto eventState = event->getSnapshot();

            const auto eventTimeInSamples =
                SeqUtil::getEventTimeInSamples(seq.get(), eventState.tick, timeInSamples, sampleRate);

            const auto renderedEventState = RenderedEventState
            {
                eventState,
                eventTimeInSamples
            };

            printf("Event tick: %lld, timeInSamples: %i\n", eventState.tick, eventTimeInSamples);

            result.events.emplace_back(std::move(renderedEventState));
        }
    }

    return result;
}
