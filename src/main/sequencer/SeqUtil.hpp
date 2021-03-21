#pragma once

namespace mpc::sequencer
{
class Sequencer;
class Sequence;
class Song;
}

namespace mpc::sequencer
{
class SeqUtil
{
    
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
    static double ticksToFrames(double ticks, const double tempo, float sr);
    
public:
    static double framesToTicks(double d, const double tempo, float sr);
    
public:
    static double sequenceFrameLength(Sequence*, int firstTick, int lastTick, float sr);
    static int loopFrameLength(Sequence*, float sr);
    static int songFrameLength(Song* song, Sequencer*, float sr);
    
};
}
