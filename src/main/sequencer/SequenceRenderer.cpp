#include "sequencer/SequenceRenderer.hpp"

#include "RenderContext.hpp"
#include "SequenceStateView.hpp"
#include "Track.hpp"
#include "TrackStateView.hpp"
#include "utils/SequencerTiming.hpp"

void mpc::sequencer::renderSeq(RenderContext &ctx)
{
    for (int i = 0; i < Mpc2000XlSpecs::TRACK_COUNT; ++i)
    {
        const auto track = ctx.seq->getTrack(i);

        for (const auto &event : track->getEvents())
        {
            if (event.type == EventType::TempoChange)
            {
                continue;
            }

            const auto eventTickToUse =
                event.tick - ctx.playbackState.lastTransitionTick;

            const auto eventTime = utils::getEventTimeInSamples(
                *ctx.seq, eventTickToUse, ctx.playbackState.strictValidFrom,
                ctx.playbackState.sampleRate);

            if (!ctx.playbackState.containsTimeInSamplesStrict(eventTime))
            {
                continue;
            }

            // event.printInfo();

            const RenderedEventState renderedEventState{event, eventTime};

            ctx.playbackState.events.emplace_back(
                std::move(renderedEventState));
        }
    }
}
