#include "utils/SequencerTiming.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/TempoChangeEvent.hpp"

mpc::utils::SequenceTimingData
mpc::utils::getSequenceTimingData(const sequencer::Sequence *seq)
{
    SequenceTimingData result;
    result.initialTempo = seq->getInitialTempo();
    result.lastTick = seq->getLastTick();
    result.loopEnabled = seq->isLoopEnabled();
    result.loopStartTick = seq->getLoopStartTick();
    result.loopEndTick = seq->getLoopEndTick();

    for (const auto e : seq->getTempoChangeEvents())
    {
        result.tempoChanges.push_back(TempoChangeEventData{
            static_cast<double>(e->getTick()), e->getTempo()});
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
