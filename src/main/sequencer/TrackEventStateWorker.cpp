#include "sequencer/TrackEventStateWorker.hpp"

#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "TrackEventStateManager.hpp"
#include "Transport.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

TrackEventStateWorker::TrackEventStateWorker(Sequencer *sequencer)
    : running(false), sequencer(sequencer)
{
}

TrackEventStateWorker::~TrackEventStateWorker()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void TrackEventStateWorker::start()
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
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
}

void TrackEventStateWorker::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
}

void TrackEventStateWorker::work() const
{
    for (int i = 0; i < Mpc2000XlSpecs::SEQUENCE_COUNT; i++)
    {
        if (!sequencer->getSequence(i)->isUsed())
        {
            continue;
        }
        for (const auto &t : sequencer->getSequence(i)->getTracks())
        {
            t->getEventStateManager()->drainQueue();
        }
    }

    if (sequencer->getPlaceHolder())
    {
        for (const auto &t : sequencer->getPlaceHolder()->getTracks())
        {
            t->getEventStateManager()->drainQueue();
        }
    }
}

PlaybackState TrackEventStateWorker::renderPlaybackState(const SampleRate sampleRate, const TimeInSamples timeInSamples) const
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
