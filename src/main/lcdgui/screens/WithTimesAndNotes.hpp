#pragma once

namespace mpc::sequencer
{
	class Sequence;
}

namespace mpc::lcdgui::screens
{
	class WithTimesAndNotes
	{

	protected:
		int time0 = 0;
		int time1 = 0;
		int note0 = 0;
		int note1 = 0;

	public:
		void setTime0(int time0);
		void setTime1(int time1);
		
		void setMidiNote0(int note);
		void setMidiNote1(int note);

	protected:
		virtual void displayNotes() = 0;
		virtual void displayTime() = 0;

	public:
		void checkAllTimes(int notch, mpc::sequencer::Sequence* sequence = nullptr);
		void checkAllTimesAndNotes(int notch, mpc::sequencer::Sequence* sequence = nullptr);

	};
}
