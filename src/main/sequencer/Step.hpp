#pragma once

namespace mpc {
	namespace sequencer {

		class Song;

		class Step
		{

		private:
			Song* song{ nullptr };
			int sequence{ 0 };
			int repeats{ 1 };

		public:
			virtual void setSequence(int i);
			virtual int getSequence();
			virtual void setRepeats(int i);
			virtual int getRepeats();

			Step(Song* song );
			~Step();

		};

	}
}
