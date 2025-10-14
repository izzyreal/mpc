#pragma once

#include <cstdint>

namespace mpc { class Mpc; }

namespace mpc::sequencer
{
class Sequencer;
class Sequence;
class Song;
}

namespace mpc::sequencer {
    class SeqUtil {
        public:
            static void setTimeSignature(Sequence*, int firstBarIndex, int lastBarIndex, int num, int den);
            static void setTimeSignature(Sequence*, int bar, int num, int den);
            static int getTickFromBar(int i, Sequence* s, int position);
            static int getBarFromTick(Sequence*, int position);
            static int getBeat(Sequence*, int position);
            static int getClock(Sequence*, int position);
            static int getBar(Sequence*, int position);
            static int setBar(int i, Sequence*, int position);
            static int setBeat(int i, Sequence*, int position);
            static int setClock(int i, Sequence*, int position);

        private:
            static double secondsPerTick(const double tempo);
            static double ticksPerSecond(const double tempo);

        public:
            static double ticksToFrames(double ticks, const double tempo, int sr);

        public:
            static double sequenceFrameLength(Sequence*, int firstTick, int lastTick, int sr);
            static int loopFrameLength(Sequence*, int sr);
            static int songFrameLength(Song* song, Sequencer*, int sr);

            static void copyBars(mpc::Mpc&,
                                 uint8_t fromSeqIndex,
                                 uint8_t toSeqIndex,
                                 uint8_t copyFirstBar,
                                 uint8_t copyLastBar,
                                 uint8_t copyCount,
                                 uint8_t copyAfterBar);

            static bool isRecMainWithoutPlaying(mpc::Mpc&);
    };
} // namespace mpc::sequencer
