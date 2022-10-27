#pragma once

namespace mpc { class Mpc; }

namespace mpc::sequencer
{
	class Sequence;
	class Track;
}

namespace mpc::lcdgui::screens
{
	class WithTimesAndNotes
	{

	protected:
		int time0 = 0;
		int time1 = 0;
		int note0 = 34;
		int note1 = 127;

	public:
		void setTime0(int time0);
		void setTime1(int time1);
		
		virtual void setNote0(int note);
		void setNote1(int note);

	protected:
		virtual void displayNotes() = 0;
		virtual void displayDrumNotes() { displayNotes(); }
		virtual void displayTime() = 0;

	public:
		bool checkAllTimes(mpc::Mpc& mpc, int notch, mpc::sequencer::Sequence* sequence = nullptr);
		bool checkAllTimesAndNotes(mpc::Mpc& mpc, int notch, mpc::sequencer::Sequence* sequence = nullptr, mpc::sequencer::Track* track = nullptr);

	};
}
