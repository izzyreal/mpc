#include "sequencer/SequenceRenderer.hpp"

#include "RenderContext.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"

void mpc::sequencer::renderSeq(RenderContext &ctx)
{
    for (const auto &track : ctx.seq->getTracks())
    {
        for (const auto &event : track->getEventStates())
        {
            if (event.type == EventType::TempoChange)
            {
                continue;
            }

            const auto eventTickToUse =
                event.tick - ctx.playbackState.originTicks;

            const TimeInSamples eventTime = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse,
                ctx.playbackState.strictValidFrom,
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
