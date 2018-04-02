#pragma once
#include <thirdp/bcmath/bcmath_stl.h>

namespace mpc {
	namespace sequencer {

		class Sequencer;
		class Sequence;
		class Song;

		class SeqUtil
		{

		public:
			static void setTimeSignature(Sequence* sequence, int firstBar, int tsLastBar, int num, int den);
			static void setTimeSignature(Sequence* sequence, int bar, int num, int den);
			static int getTickFromBar(int i, Sequence* s, int position);
			static int setBeat(int i, Sequence* s, int position);
			static int setClockNumber(int i, Sequence* s, int position);
			static int getBarFromTick(Sequence* s, int position);
			static int getBeat(Sequence* s, int position);
			static int getClockNumber(Sequence* s, int position);

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

			SeqUtil();

		};

	}
}
