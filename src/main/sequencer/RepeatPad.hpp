#pragma once

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class RepeatPad
    {
    public:
        static void process(mpc::Mpc &,
                            unsigned int tickPosition,
                            int durationTicks,
                            unsigned short eventFrameOffset,
                            double tempo,
                            float sampleRate);
    };
} // namespace mpc::sequencer