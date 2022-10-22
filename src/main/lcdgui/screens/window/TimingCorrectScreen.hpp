#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window
{
	class TimingCorrectScreen
		: public mpc::lcdgui::ScreenComponent
		, public mpc::lcdgui::screens::WithTimesAndNotes
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		TimingCorrectScreen(mpc::Mpc& mpc, const int layerIndex);

	public:
		void open() override;

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

	public:
		int getNoteValue();
		void setNoteValue(int i);
		int getSwing();
        int getAmount();
        int isShiftTimingLater();

	private:
		void setAmount(int i);
		void setShiftTimingLater(bool b);
		void setSwing(int i);
	};
}
