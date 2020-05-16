#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window {

	class TimingCorrectScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		TimingCorrectScreen(const int& layer);

	public:
		void open() override;

	private:
		void displayNoteValue();
		void displaySwing();
		void displayNotes();
		void displayTime();
		void displayShiftTiming();
		void displayAmount();

	private:
		std::vector<std::string> noteValueNames = { "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };
		bool shiftTimingLater = false;
		int swing = 50;
		int amount = 0;
		int noteValue = 3;
		int midiNote0 = 0;
		int midiNote1 = 127;
		int time0 = 0;
		int time1 = 0;

	public:
		int getNoteValue();
		void setNoteValue(int i);
		int getSwing();

	private:
		void setAmount(int i);
		void setShiftTimingLater(bool b);
		void setMidiNote0(int i);
		void setMidiNote1(int i);
		void setSwing(int i);
		void setTime0(int time0);
		void setTime1(int time1);

	};
}
