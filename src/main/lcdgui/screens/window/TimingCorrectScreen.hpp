#pragma once
#include <lcdgui/screens/window/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window {

	class TimingCorrectScreen
		: public WithTimesAndNotes
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		TimingCorrectScreen(const int& layer);

	public:
		void open() override;
		void close() override;

	private:
		void displayNoteValue();
		void displaySwing();
		void displayShiftTiming();
		void displayAmount();

	protected:
		void displayNotes() override;
		void displayTime() override;

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
		void setSwing(int i);
		
	public:
		void update(moduru::observer::Observable* observable, nonstd::any message);
	};
}
