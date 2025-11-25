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
            const auto playbackState = renderPlaybackState(
                SampleRate(sampleRate), currentTimeInSamples);
            sequencer->getNonRtStateManager()->enqueue(
                UpdatePlaybackState{std::move(playbackState)});
        }
        else
        {
            sequencer->getNonRtStateManager()->enqueue(
                UpdatePlaybackState{PlaybackState()});
        }
    }

    sequencer->getNonRtStateManager()->drainQueue();
}

void NonRtSequencerStateWorker::refreshPlaybackState() const
{
    const auto playbackEngine = sequencer->getSequencerPlaybackEngine();
    const auto currentTimeInSamples = playbackEngine->getCurrentTimeInSamples();
    const auto sampleRate = playbackEngine->getSampleRate();
    const auto playbackState =
        renderPlaybackState(SampleRate(sampleRate), currentTimeInSamples);
    sequencer->getNonRtStateManager()->enqueue(
        UpdatePlaybackState{std::move(playbackState)});
}

struct RenderContext
{
    PlaybackState playbackState;
    Sequence *seq;
    SequenceStateView seqSnapshot;
};

void renderMetronome(const CountMetronomeScreen *screen, RenderContext &ctx)
{
    const auto metronomeRate = screen->getRate();

    int barTickOffset = 0;

    for (int i = 0; i < ctx.seq->getBarCount(); ++i)
    {
        const auto ts = ctx.seqSnapshot.getTimeSignature(i);
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
            const auto eventTimeInSamples = SeqUtil::getEventTimeInSamples(
                ctx.seq, j + barTickOffset, ctx.playbackState.currentTime,
                ctx.playbackState.sampleRate);

            if (eventTimeInSamples > ctx.playbackState.validUntil)
            {
                continue;
            }

            EventState eventState;
            eventState.tick = j;
            eventState.type = EventType::MetronomeClick;
            eventState.velocity =
                j == 0 ? mpc::MaxVelocity : mpc::MediumVelocity;
            ctx.playbackState.events.emplace_back(
                RenderedEventState{std::move(eventState), eventTimeInSamples});
        }

        barTickOffset += barLength;
    }
}

void renderSeq(RenderContext &ctx)
{
    for (const auto &track : ctx.seq->getTracks())
    {
        for (const auto &event : track->getEvents())
        {
            const auto eventState = event->getSnapshot();

            const mpc::TimeInSamples eventTime = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventState.tick, ctx.playbackState.currentTime,
                ctx.playbackState.sampleRate);

            if (eventTime > ctx.playbackState.validUntil)
            {
                continue;
            }

            const RenderedEventState renderedEventState{eventState, eventTime};

            ctx.playbackState.events.emplace_back(
                std::move(renderedEventState));
        }
    }
}

PlaybackState NonRtSequencerStateWorker::renderPlaybackState(
    const SampleRate sampleRate, const TimeInSamples currentTime) const
{
    constexpr TimeInSamples snapshotWindowSize{44100 * 2};

    const TimeInSamples validUntil = currentTime + snapshotWindowSize;

    const auto seqIndex = sequencer->isSongModeEnabled()
                          ? sequencer->getSongSequenceIndex()
                          : sequencer->getSelectedSequenceIndex();

    const auto seq = sequencer->getSequence(seqIndex);

    const auto seqSnapshot = seq->getStateManager()->getSnapshot();

    PlaybackState playbackState{sampleRate, currentTime, validUntil};

    RenderContext renderCtx{
        std::move(playbackState),
        seq.get(), std::move(seqSnapshot)
    };

    renderSeq(renderCtx);

    const auto countMetronomeScreen =
        sequencer->getScreens()
            ->get<lcdgui::ScreenId::CountMetronomeScreen>()
            .get();

    renderMetronome(countMetronomeScreen, renderCtx);

    return renderCtx.playbackState;
}
