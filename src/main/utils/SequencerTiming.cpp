#include "utils/SequencerTiming.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SequenceStateView.hpp"

mpc::utils::SequenceTimingData
mpc::utils::getSequenceTimingData(const sequencer::SequenceStateView &seq)
{
    SequenceTimingData result;
    result.initialTempo = seq.getInitialTempo();
    result.lastTick = seq.getLastTick();
    result.loopEnabled = seq.isLoopEnabled();
    result.loopStartTick = seq.getLoopStartTick();
    result.loopEndTick = seq.getLoopEndTick();

    for (const auto e : seq.getTempoChangeEvents())
    {
        const auto tempo = result.initialTempo * e.amount * 0.001;
        result.tempoChanges.push_back(
            TempoChangeEventData{static_cast<double>(e.tick), tempo});
    }

    return result;
}

double mpc::utils::getTickCountForFrames(const SequenceTimingData &s,
                                         const double firstTick,
                                         const int frameCount, const int sr)
{
    const auto &tces = s.tempoChanges;
    const int n = tces.size();

    auto secondsPerTick = [&](const double tempo)
    {
        return 60.0 / tempo / Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ;
    };

    auto findTempoAt = [&](const double tick)
    {
        double tempo = s.initialTempo;
        for (int i = 0; i < n && tces[i].tick <= tick; i++)
        {
            tempo = tces[i].tempo;
        }
        return tempo;
    };

    auto advanceThroughTempoMap = [&](const double tick,
                                      const double frames) -> double
    {
        double remaining = frames;
        double t = tick;
        double tempo = findTempoAt(t);

        int i = 0;
        while (i < n && tces[i].tick <= t)
        {
            i++;
        }

        while (i < n && remaining > 0.0)
        {
            const double segEnd = tces[i].tick;
            const double avail = segEnd - t;
            if (avail > 0.0)
            {
                const double spt = secondsPerTick(tempo);
                const double fpt = spt * sr;
                const double framesSeg = avail * fpt;
                if (framesSeg > remaining)
                {
                    return remaining / fpt;
                }
                remaining -= framesSeg;
                t = segEnd;
            }
            tempo = tces[i].tempo;
            i++;
        }

        if (remaining > 0.0)
        {
            const double spt = secondsPerTick(tempo);
            const double fpt = spt * sr;
            return remaining / fpt;
        }

        return 0.0;
    };

    const bool loop = s.loopEnabled;
    const double loopStart = loop ? s.loopStartTick : 0.0;
    const double loopEnd = loop ? s.loopEndTick : s.lastTick;

    double framesLeft = frameCount;
    double currentTick = firstTick;
    double totalTicks = 0.0;

    if (!loop)
    {
        return advanceThroughTempoMap(currentTick, framesLeft);
    }

    if (loopEnd - loopStart == 0)
    {
        return 0.0;
    }

    if (currentTick < loopStart)
    {
        const double toLoopStart = loopStart - currentTick;
        const double ticks = advanceThroughTempoMap(currentTick, framesLeft);
        if (ticks < toLoopStart)
        {
            return ticks;
        }
        const double spt = secondsPerTick(findTempoAt(currentTick));
        const double fpt = spt * sr;
        const double framesUsed = toLoopStart * fpt;
        framesLeft -= framesUsed;
        totalTicks += toLoopStart;
        currentTick = loopStart;
    }
    else if (currentTick > loopEnd)
    {
        const double toEnd = s.lastTick - currentTick;
        const double ticks = advanceThroughTempoMap(currentTick, framesLeft);
        if (ticks < toEnd)
        {
            return ticks;
        }
        const double spt = secondsPerTick(findTempoAt(currentTick));
        const double fpt = spt * sr;
        const double framesUsed = toEnd * fpt;
        framesLeft -= framesUsed;
        totalTicks += toEnd;
        currentTick = loopStart;
    }

    while (framesLeft > 0.0)
    {
        const double toLoopEnd = loopEnd - currentTick;
        const double ticks = advanceThroughTempoMap(currentTick, framesLeft);
        if (ticks < toLoopEnd)
        {
            return totalTicks + ticks;
        }
        const double spt = secondsPerTick(findTempoAt(currentTick));
        const double fpt = spt * sr;
        const double framesUsed = toLoopEnd * fpt;
        framesLeft -= framesUsed;
        totalTicks += toLoopEnd;
        currentTick = loopStart;
    }

    return totalTicks;
}

int mpc::utils::getFrameCountForTicks(const SequenceTimingData &s,
                                      const double firstTick,
                                      const double tickCount, const int sr)
{
    const auto &tces = s.tempoChanges;
    const int n = tces.size();

    auto secondsPerTick = [&](const double tempo)
    {
        return 60.0 / tempo / Mpc2000XlSpecs::SEQUENCER_RESOLUTION_PPQ;
    };

    auto findTempoAt = [&](const double tick)
    {
        double tempo = s.initialTempo;
        for (int i = 0; i < n && tces[i].tick <= tick; i++)
        {
            tempo = tces[i].tempo;
        }
        return tempo;
    };

    double remaining = tickCount;
    double t = firstTick;
    double frames = 0.0;
    double tempo = findTempoAt(t);

    int i = 0;
    while (i < n && tces[i].tick <= t)
    {
        i++;
    }

    while (i < n && remaining > 0.0)
    {
        const double segEnd = tces[i].tick;
        double avail = segEnd - t;
        if (avail > 0.0)
        {
            const double used = std::min(avail, remaining);
            const double spt = secondsPerTick(tempo);
            frames += used * (spt * sr);
            remaining -= used;
            t += used;
        }
        tempo = tces[i].tempo;
        i++;
    }

    if (remaining > 0.0)
    {
        const double spt = secondsPerTick(tempo);
        frames += remaining * (spt * sr);
    }

    return static_cast<int>(std::ceil(frames));
}

int mpc::utils::getEventTimeInSamples(const sequencer::SequenceStateView &seq,
                                      const int eventTick,
                                      const int currentTimeSamples,
                                      const SampleRate sampleRate)
{
    const auto seqTimingData = getSequenceTimingData(seq);
    const auto seqFrameLength = getFrameCountForTicks(seqTimingData, 0, seq.getLastTick(), sampleRate);
    const int loopLen = seqFrameLength;

    if (loopLen <= 0)
    {
        return currentTimeSamples;
    }

    const int phase = currentTimeSamples % loopLen;

    const int eventSample = getFrameCountForTicks(seqTimingData, 0, eventTick, sampleRate);

    if (eventSample >= phase)
    {
        return currentTimeSamples + (eventSample - phase);
    }

    return currentTimeSamples + (loopLen - (phase - eventSample));
}

int mpc::utils::getEventTimeInSamples(
    const SequenceTimingData &snapshotTimingData,
    int blockStartTick,
    int eventTick,
    int64_t strictValidFromSamples,
    SampleRate sampleRate)
{
    // delta ticks from block start
    const double deltaTicks = static_cast<double>(eventTick - blockStartTick);

    // convert delta ticks to samples using snapshot tempo map
    const double deltaFrames = getFrameCountForTicks(
        snapshotTimingData, static_cast<double>(blockStartTick),
        deltaTicks, sampleRate);

    // event sample = block start + deltaFrames
    return static_cast<int>(strictValidFromSamples + std::round(deltaFrames));
}