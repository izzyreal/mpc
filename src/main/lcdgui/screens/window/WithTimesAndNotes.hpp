#pragma once

#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

	class WithTimesAndNotes
		: virtual public mpc::lcdgui::ScreenComponent
	{

	protected:
		int time0 = 0;
		int time1 = 0;
		int note0 = 0;
		int note1 = 0;

	public:
		void setTime0(int time0);
		void setTime1(int time1);
		
		int getMidiNote0();
		int getMidiNote1();
		void setMidiNote0(int note);
		void setMidiNote1(int note);

	protected:
		virtual void displayNotes() = 0;
		virtual void displayTime() = 0;

	public:
		void checkAllTimesAndNotes(int notch);

	};
}
