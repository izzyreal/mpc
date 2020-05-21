#pragma once
#include <thirdp/bcmath/bcmath_stl.h>

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
		static void setTimeSignature(Sequence* sequence, int firstBar, int tsLastBar, int num, int den);
		static void setTimeSignature(Sequence* sequence, int bar, int num, int den);
		static int getTickFromBar(int i, Sequence* s, int position);
		static int getBarFromTick(Sequence* s, int position);
		static int getBeat(Sequence* s, int position);
		static int getClock(Sequence* s, int position);
		static int getBar(mpc::sequencer::Sequence* s, int position);
		static int setBar(int i, mpc::sequencer::Sequence* s, int position);
		static int setBeat(int i, Sequence* s, int position);
		static int setClock(int i, Sequence* s, int position);

	private:
		static double secondsPerTick(BCMath tempo);
		static double ticksPerSecond(BCMath tempo);

	public:
		static double ticksToFrames(double ticks, BCMath tempo, float sr);

	public:
		static double framesToTicks(double d, BCMath tempo, float sr);

	public:
		static double sequenceFrameLength(Sequence* seq, int firstTick, int lastTick, float sr);
		static int loopFrameLength(Sequence* seq, float sr);
		static int songFrameLength(Song* song, mpc::sequencer::Sequencer* sequencer, float sr);

	};
}
