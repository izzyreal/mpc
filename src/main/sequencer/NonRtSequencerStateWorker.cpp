#include "sequencer/NonRtSequencerStateWorker.hpp"

#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "NonRtSequencerStateManager.hpp"
#include "Transport.hpp"
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

void NonRtSequencerStateWorker::work() const
{
    sequencer->getNonRtStateManager()->drainQueue();
}

PlaybackState NonRtSequencerStateWorker::renderPlaybackState(const SampleRate sampleRate, const TimeInSamples timeInSamples) const
{
    constexpr TimeInSamples snapshotWindowSizeSamples{30'000};

    PlaybackState result;
    result.timeInSamples = timeInSamples;

    const Tick currentTickPosition = sequencer->getTransport()->getTickPosition();

    const auto seqIndex = sequencer->isSongModeEnabled() ? sequencer->getSongSequenceIndex()
    : sequencer->getSelectedSequenceIndex();
    const auto seq = sequencer->getSequence(seqIndex);

    const Tick snapshotWindowInTicks =
        SeqUtil::getTickCountForFrames(
            seq.get(),
            currentTickPosition,
            snapshotWindowSizeSamples,
            sampleRate);

    const Tick lastTickToIncludeInSnapshot = currentTickPosition + snapshotWindowInTicks;

    std::vector<std::shared_ptr<Track>> tracksToProcess = seq->getTracks();

    tracksToProcess.push_back(seq->getTempoChangeTrack());

    for (const auto &track : tracksToProcess)
    {
        while (track->getNextEventTick() <= lastTickToIncludeInSnapshot)
        {
            auto eventState = track->getNextEventAndIncrementEventIndex();

            if (!eventState) continue;

            const auto eventTimeInSamples =
                SeqUtil::getEventTimeInSamples(seq.get(), eventState->tick, currentTickPosition, timeInSamples, sampleRate);

            const auto renderedEventState = RenderedEventState
            {
                std::move(*eventState),
                eventTimeInSamples
            };

            result.events.emplace_back(std::move(renderedEventState));
        }

        if (lastTickToIncludeInSnapshot > seq->getLastTick())
        {
            constexpr Tick loopStart = 0;
            track->syncEventIndex(loopStart, seq->getLastTick());

            while (track->getNextEventTick() <= lastTickToIncludeInSnapshot % seq->getLastTick())
            {
                track->getNextEventTick();
            }
        }
    }

    return result;
}
