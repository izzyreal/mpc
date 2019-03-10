#pragma once
#include <observer/Observable.hpp>


namespace mpc {

	namespace sequencer {
		class Sequence;
	}

	namespace ui {
		namespace sequencer {

			class EditSequenceGui
				: public moduru::observer::Observable
			{

			public:
				typedef moduru::observer::Observable super;

			private:
				bool modeMerge{};

				int time0{ 0 };
				int time1{ 0 };
				int editFunctionNumber{ 0 };
				int drumNote{ 0 };
				int fromSq{ 0 };
				int tr0{ 0 };
				int toSq{ 0 };
				int tr1{ 0 };
				int startTicks{ 0 };
				int copies{ 0 };
				int midiNote0{ 0 };
				int midiNote1{ 0 };
				int durationMode{ 0 };
				int velocityMode{ 0 };
				int transposeAmount{ 0 };
				int durationValue{ 0 };
				int velocityValue{ 0 };
			
			public:
				void setEditFunctionNumber(int i);
				int getEditFunctionNumber();
				int getDrumNote();
				void setDrumNote(int i);
				int getMidiNote0();
				void setMidiNote0(int i);
				int getMidiNote1();
				void setMidiNote1(int i);
				int getFromSq();
				void setFromSq(int i);
				int getTr0();
				void setTr0(int i);
				int getToSq();
				void setToSq(int i);
				int getTr1();
				void setTr1(int i);
				bool isModeMerge();
				void setModeMerge(bool b);
				int getCopies();
				void setCopies(int i);
				void setDurationMode(int i);
				int getDurationMode();
				void setVelocityMode(int i);
				int getVelocityMode();
				void setTransposeAmount(int i);
				int getTransposeAmount();
				int getDurationValue();
				void setDurationValue(int i);
				int getVelocityValue();
				void setVelocityValue(int i);
				int getTime0();
				void setTime0(int time0);
				int getTime1();
				void setTime1(int time1);
				int getStartTicks();
				void setStartTicks(int startTicks);
				int setBarNumber(int i, mpc::sequencer::Sequence* s, int position);
				int setBeatNumber(int i, mpc::sequencer::Sequence* s, int position);
				int setClockNumber(int i, mpc::sequencer::Sequence* s, int position);
				static int getBarNumber(mpc::sequencer::Sequence* s, int position);
				static int getBeatNumber(mpc::sequencer::Sequence* s, int position);
				static int getClockNumber(mpc::sequencer::Sequence* s, int position);

				EditSequenceGui();
			};

		}
	}
}
