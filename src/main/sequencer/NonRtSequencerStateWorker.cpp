#include "sequencer/NonRtSequencerStateWorker.hpp"

#include "Event.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "NonRtSequencerStateManager.hpp"
#include "SequenceStateManager.hpp"
#include "TimeSignature.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
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
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
    if (const auto currentTimeInSamples =
            sequencer->getSequencerPlaybackEngine()->getCurrentTimeInSamples();
        lastRenderedTimeInSamples != currentTimeInSamples)
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

void NonRtSequencerStateWorker::refreshPlaybackState() const
{
    const auto playbackEngine = sequencer->getSequencerPlaybackEngine();
    const auto currentTimeInSamples =
            playbackEngine->getCurrentTimeInSamples();
    const auto sampleRate = playbackEngine->getSampleRate();
    const auto playbackState = renderPlaybackState(SampleRate(sampleRate), currentTimeInSamples);
    sequencer->getNonRtStateManager()->enqueue(UpdatePlaybackState{std::move(playbackState)});
}

PlaybackState NonRtSequencerStateWorker::renderPlaybackState(const SampleRate sampleRate, const TimeInSamples timeInSamples) const
{
    constexpr TimeInSamples snapshotWindowSizeSamples{44100 * 2};

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

            if (eventTimeInSamples > timeInSamples + snapshotWindowSizeSamples)
            {
                continue;
            }

            const RenderedEventState renderedEventState
            {
                eventState,
                eventTimeInSamples
            };

            result.events.emplace_back(std::move(renderedEventState));
        }
    }

    const auto seqSnapshot = seq->getStateManager()->getSnapshot();
    const auto countMetronomeScreen = sequencer->getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>();
    const auto metronomeRate = countMetronomeScreen->getRate();

    int barTickOffset = 0;

    for (int i = 0; i < seq->getBarCount(); ++i)
    {
        const auto ts = seqSnapshot.getTimeSignature(i);
        const auto den = ts.denominator;
        auto denTicks = 96 * (4.0 / den);

        switch (metronomeRate)
        {
            case 1:
                denTicks *= 2.0f / 3.f;
                break;
            case 2:
                denTicks *= 1.0f / 2;
                break;
            case 3:
                denTicks *= 1.0f / 3;
                break;
            case 4:
                denTicks *= 1.0f / 4;
                break;
            case 5:
                denTicks *= 1.0f / 6;
                break;
            case 6:
                denTicks *= 1.0f / 8;
                break;
            case 7:
                denTicks *= 1.0f / 12;
                break;
            default:;
        }

        const auto barLength = ts.getBarLength();

        for (int j = 0; j < barLength; j += denTicks)
        {
            const auto eventTimeInSamples =
                SeqUtil::getEventTimeInSamples(seq.get(), j + barTickOffset, timeInSamples, sampleRate);

            if (eventTimeInSamples > timeInSamples + snapshotWindowSizeSamples)
            {
                continue;
            }

            EventState eventState;
            eventState.tick = j;
            eventState.type = EventType::MetronomeClick;
            eventState.velocity = j == 0 ? MaxVelocity : MediumVelocity;
            result.events.emplace_back(RenderedEventState{std::move(eventState), eventTimeInSamples});
        }

        barTickOffset += barLength;
    }

    return result;
}
