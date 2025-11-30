#pragma once

#include "IntTypes.hpp"

#include <memory>
#include <string>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class Sequencer;
    class Sequence;
    class Song;
} // namespace mpc::sequencer

namespace mpc::lcdgui::screens::window
{
    class TimingCorrectScreen;
}

namespace mpc::hardware
{
    class Button;
}

namespace mpc::controller
{
    class ClientHardwareEventController;
}

namespace mpc::sequencer
{
    class SeqUtil
    {
    public:
        static int getTickFromBar(int i, const Sequence *s, int position);
        static int getBarFromTick(const Sequence *, int position);
        static int getBeat(const Sequence *, int position);
        static int getClock(const Sequence *, int position);
        static int getBar(const Sequence *, int position);
        static int setBar(int i, const Sequence *, int position);
        static int setBeat(int i, const Sequence *, int position);
        static int setClock(int i, const Sequence *, int position);

    private:
        static double secondsPerTick(double tempo);
        static double ticksPerSecond(double tempo);

    public:
        static double ticksToFrames(double ticks, double tempo, int sampleRate);
        static double framesToTicks(double frames, double tempo,
                                    int sampleRate);

        static double sequenceFrameLength(const Sequence *, int firstTick,
                                          int lastTick, int sr);
        static int loopFrameLength(const Sequence *, int sr);
        static int songFrameLength(Song *song, Sequencer *, int sr);

        static void copyBars(Mpc &, uint8_t fromSeqIndex, uint8_t toSeqIndex,
                             uint8_t copyFirstBar, uint8_t copyLastBar,
                             uint8_t copyCount, uint8_t copyAfterBar);

        static bool isRecMainWithoutPlaying(
            const std::shared_ptr<Sequencer> &sequencer,
            const std::shared_ptr<lcdgui::screens::window::TimingCorrectScreen>
                &timingCorrectScreen,
            const std::string &currentScreenName,
            const std::shared_ptr<hardware::Button> &recButton,
            const std::shared_ptr<controller::ClientHardwareEventController>
                &clientHardwareEventController);

        static bool isStepRecording(const std::string &currentScreenName,
                                    const std::shared_ptr<Sequencer> &);
    };
} // namespace mpc::sequencer
